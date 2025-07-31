#include "utils/draw.hpp"
#include "vision/camera.hpp"
#include "vision/tracker.hpp"
#include "vision/predictor.hpp"
#include "vision/bridge.hpp"
#include <deque>

inline std::mutex mutex;
inline std::atomic has_sent{false};
inline std::atomic stop_all{false};
inline std::optional<Bridge::Payload> shared_payload;

class VisionEnd {
public:
	Camera cam_top = Camera(CameraType::TOP, {0, cv::CAP_MSMF}, 60);
	Camera cam_left = Camera(CameraType::LEFT, {1, cv::CAP_MSMF}, 60);
	Camera cam_right = Camera(CameraType::RIGHT, {2, cv::CAP_MSMF}, 60);
	Tracker tracker_top = Tracker(ORANGE_MIN, ORANGE_MAX);
	Tracker tracker_left = Tracker(ORANGE_MIN, ORANGE_MAX);
	Tracker tracker_right = Tracker(ORANGE_MIN, ORANGE_MAX);
	Predictor predictor;

	std::mutex frame_mutex;
	cv::Mat latest_top_frame, latest_left_frame, latest_right_frame;

	VisionEnd() {
		cam_top.set_frame_callback([this](cv::Mat& frame) {
			std::lock_guard lock(frame_mutex);
			tracker_top << frame;
			Draw::put_text_border(
				frame, std::format("FPS: {:.2f}/{:.2f}", cam_top.get_fps(), cam_top.get_prop(cv::CAP_PROP_FPS)),
				{10, 20}, COLOR_WHITE);

			const auto pos = tracker_top.get_camera_pos();
			if (pos) {
				predictor.set_point_top(pos.value());
				Draw::put_circle(frame, pos.value(), 5, COLOR_GREEN);
			}

			frame.copyTo(latest_top_frame);
		});
		cam_left.set_frame_callback([this](cv::Mat& frame) {
			std::lock_guard lock(frame_mutex);
			tracker_left << frame;
			Draw::put_text_border(
				frame, std::format("FPS: {:.2f}/{:.2f}", cam_top.get_fps(), cam_top.get_prop(cv::CAP_PROP_FPS)),
				{10, 20}, COLOR_WHITE);

			const auto pos = tracker_left.get_camera_pos();
			if (pos) {
				predictor.set_point_left(pos.value());
				Draw::put_circle(frame, pos.value(), 5, COLOR_GREEN);
			}

			frame.copyTo(latest_left_frame);
		});
		cam_right.set_frame_callback([this](cv::Mat& frame) {
			std::lock_guard lock(frame_mutex);
			tracker_right << frame;
			Draw::put_text_border(
				frame, std::format("FPS: {:.2f}/{:.2f}", cam_top.get_fps(), cam_top.get_prop(cv::CAP_PROP_FPS)),
				{10, 20}, COLOR_WHITE);

			const auto pos = tracker_right.get_camera_pos();
			if (pos) {
				predictor.set_point_right(pos.value());
				Draw::put_circle(frame, pos.value(), 5, COLOR_GREEN);
			}

			frame.copyTo(latest_right_frame);
		});
	}

	~VisionEnd() {
		cam_top.stop();
		cam_left.stop();
		cam_right.stop();
		cv::destroyAllWindows();
	}

	void run() {
		if (!cam_top.is_opened() || !cam_left.is_opened() || !cam_right.is_opened()) {
			const std::string message = std::format(
				"Failed to open camera: top={}, left={}, right={}",
				cam_top.is_opened() ? "true" : "false",
				cam_left.is_opened() ? "true" : "false",
				cam_right.is_opened() ? "true" : "false"
			);
			std::cerr << message << std::endl;
			return;
		}

		cam_top.start();
		cam_left.start();
		cam_right.start();

		std::deque<cv::Point3f> orbit;
		cv::Point3f world_pos{-1, -1, -1};
		cv::Vec3f world_speed{0, 0, 0};
		cv::Point3f predict_pos{-1, -1, -1};
		cv::Point3f real_arrive_pos{-1, -1, -1};

		while (!stop_all.load()) {
#pragma region FRAME_READ
			cv::Mat frame_top, frame_left, frame_right;
			{
				std::lock_guard lock(frame_mutex);
				if (latest_top_frame.empty() || latest_left_frame.empty() || latest_right_frame.empty())
					continue;

				latest_top_frame.copyTo(frame_top);
				latest_left_frame.copyTo(frame_left);
				latest_right_frame.copyTo(frame_right);
			}

			if (frame_top.empty() || frame_left.empty() || frame_right.empty())
				continue;
#pragma endregion

#pragma region DATA_PROCESSING
			// TODO: 한 번 탑 카메라 아예 안 써보는 건 어떰?
			// world_pos 구하기
			const double fallback_dt = 1.0 / std::min({cam_top.get_fps(), cam_left.get_fps(), cam_right.get_fps()});
			if (const auto find_world_pos = predictor.get_new_world_pos(static_cast<float>(fallback_dt))) {
				world_pos = find_world_pos.value();

				orbit.push_back(world_pos);
				if (orbit.size() > 100)
					orbit.pop_front();
			}

			// world_speed 구하기
			if (const auto find_world_speed = predictor.get_world_pos()) {
				world_speed = find_world_speed.value();
			}

			// TODO: predict 실패하는 경우 조사하기 + 로그 상세히
			// 예상 도착 위치 구하기
			if (PREDICT_MIN_Y <= world_pos.y && world_pos.y <= PREDICT_MAX_Y) {
				if (const auto find_predict_pos = predictor.get_arrive_pos()) {
					predict_pos = find_predict_pos.value();
				}
				has_sent = false;
			}
			else if (0 <= world_pos.x && world_pos.x <= TABLE_WIDTH && 0 <= world_pos.y && 0 <= world_pos.z && !has_sent.load()) {
				std::lock_guard lock(mutex);

				// TODO: angle 계산 & 마지막 도착 직전 속도 넣기? 근데 이건 불가능 로봇 반응속도가 그래도 계산은 가능하지
				auto payload = Bridge::convert(predict_pos, world_speed, 35);

				std::cout << "Predicted position: " << predict_pos << std::endl;
				std::cout << "Broadcast to hardware: " << std::endl
					<< "\tx: " << payload.x << " cm" << std::endl
					<< "\ttheta: " << payload.theta << " deg" << std::endl
					<< "\tswing_start: " << payload.swing_start << " deg" << std::endl
					<< "\tswing_end: " << payload.swing_end << " deg" << std::endl
					<< "\twrist_angle: " << payload.wrist_angle << " deg" << std::endl;

				shared_payload = payload;
				has_sent = true;
			}

			// 실제 도착 위치 구하기
			if (0 <= world_pos.x && world_pos.x <= TABLE_WIDTH && std::abs(world_pos.y) < 5 && world_pos.z >= 0) {
				real_arrive_pos = world_pos;
			}
#pragma endregion

#pragma region DRAW
			// 궤적 그리기
			for (const auto& pos : orbit) {
				Draw::put_circle(frame_top, predictor.pos_3d_to_2d(pos, CameraType::TOP), 3, COLOR_MAGENTA);
				Draw::put_circle(frame_left, predictor.pos_3d_to_2d(pos, CameraType::LEFT), 3, COLOR_MAGENTA);
				Draw::put_circle(frame_right, predictor.pos_3d_to_2d(pos, CameraType::RIGHT), 3, COLOR_MAGENTA);
			}

			// 예측 도착 위치 그리기
			if (0 <= predict_pos.x && predict_pos.x <= TABLE_WIDTH && predict_pos.z >= 0) {
				Draw::put_circle(frame_top, predictor.pos_3d_to_2d(predict_pos, CameraType::TOP), 10, COLOR_BLUE);
				Draw::put_circle(frame_left, predictor.pos_3d_to_2d(predict_pos, CameraType::LEFT), 10, COLOR_BLUE);
				Draw::put_circle(frame_right, predictor.pos_3d_to_2d(predict_pos, CameraType::RIGHT), 10, COLOR_BLUE);
			}

			// 실제 도착 위치 그리기
			if (0 <= real_arrive_pos.x && real_arrive_pos.x <= TABLE_WIDTH && real_arrive_pos.z >= 0) {
				Draw::put_circle(frame_top, predictor.pos_3d_to_2d(real_arrive_pos, CameraType::TOP), 10, COLOR_RED);
				Draw::put_circle(frame_left, predictor.pos_3d_to_2d(real_arrive_pos, CameraType::LEFT), 10, COLOR_RED);
				Draw::put_circle(frame_right, predictor.pos_3d_to_2d(real_arrive_pos, CameraType::RIGHT), 10, COLOR_RED);
			}
#pragma endregion

#pragma region IMSHOW
			cv::Mat frame_top_resized, frame_left_resized, frame_right_resized;
			cv::resize(frame_top, frame_top_resized, {}, 0.7, 0.7);
			cv::resize(frame_left, frame_left_resized, {}, 0.8, 0.8);
			cv::resize(frame_right, frame_right_resized, {}, 0.8, 0.8);

			int max_width = std::max(frame_left_resized.cols + frame_right_resized.cols, frame_top_resized.cols);
			int total_height = frame_top_resized.rows + std::max(frame_left_resized.rows, frame_right_resized.rows);

			cv::Mat final_frame = cv::Mat::zeros(total_height, max_width, frame_left_resized.type());
#pragma region LEGEND
			Draw::put_text_border(final_frame, Draw::to_string("World Pos", world_pos, "cm"), {10, 50}, COLOR_GREEN);
			Draw::put_text_border(final_frame, Draw::to_string("World Speed", world_speed, "cm/s"), {10, 80},
			                      COLOR_CYAN);
			Draw::put_text_border(final_frame, Draw::to_string("Predict Pos", predict_pos, "cm"), {10, 110},
			                      COLOR_BLUE);
			Draw::put_text_border(final_frame, Draw::to_string("Real Arrive Pos", real_arrive_pos, "cm"), {10, 140},
			                      COLOR_RED);
#pragma endregion
			int top_x_offset = (max_width - frame_top_resized.cols) / 2;

			frame_top_resized.copyTo(final_frame(cv::Rect(
				top_x_offset, 0, frame_top_resized.cols, frame_top_resized.rows)));
			frame_left_resized.copyTo(final_frame(cv::Rect(
				0, frame_top_resized.rows, frame_left_resized.cols, frame_left_resized.rows)));
			frame_right_resized.copyTo(final_frame(cv::Rect(
				frame_left_resized.cols, frame_top_resized.rows, frame_right_resized.cols, frame_right_resized.rows)));

			cv::imshow("Combined", final_frame);
#pragma endregion
			if (cv::waitKey(1) == 'q') {
				stop_all = true;
				break;
			}
		}
	}
};

int main() {
	VisionEnd vision_end;
	std::thread vision_thread(&VisionEnd::run, &vision_end);

	vision_thread.join();

	return 0;
}
