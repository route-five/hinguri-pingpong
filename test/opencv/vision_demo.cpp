//#include "vision/camera.hpp"
//#include "utils/draw.hpp"
//
//int main() {
//	Camera cam_top(CameraType::TOP, { 0, cv::CAP_MSMF }, 60);
//	Camera cam_left(CameraType::LEFT, { 1, cv::CAP_MSMF }, 60);
//	Camera cam_right(CameraType::RIGHT, { 2, cv::CAP_MSMF }, 60);
//
//	cam_top.set_frame_callback([&cam_top](cv::Mat& frame) {
//		if (frame.empty()) return;
//
//		Draw::put_text_border(frame, std::format("FPS: {}/{}", cam_top.get_fps(), cam_top.get_prop(cv::CAP_PROP_FPS)), { 10, 30 }, COLOR_WHITE);
//	});
//
//	cam_left.set_frame_callback([&cam_left](cv::Mat& frame) {
//		if (frame.empty()) return;
//
//		Draw::put_text_border(frame, std::format("FPS: {}/{}", cam_left.get_fps(), cam_left.get_prop(cv::CAP_PROP_FPS)), { 10, 30 }, COLOR_WHITE);
//	});
//
//	cam_right.set_frame_callback([&cam_right](cv::Mat& frame) {
//		if (frame.empty()) return;
//
//		Draw::put_text_border(frame, std::format("FPS: {}/{}", cam_right.get_fps(), cam_right.get_prop(cv::CAP_PROP_FPS)), {10, 30}, COLOR_WHITE);
//	});
//	
//	if (!cam_left.is_opened() || !cam_right.is_opened()) {
//		std::cerr << "Can't open camera" << std::endl;
//		return 1;
//	}
//
//	cam_top.start();
//	cam_left.start();
//	cam_right.start();
//
//	while (true) {
//		cv::Mat frame_top, frame_left, frame_right;
//		cam_top >> frame_top;
//		cam_left >> frame_left;
//		cam_right >> frame_right;
//
//		if (frame_top.empty() || frame_left.empty() || frame_right.empty()) continue;
//
//		cv::imshow("frame_top", frame_top);
//		cv::imshow("frame_left", frame_left);
//		cv::imshow("frame_right", frame_right);
//		
//		if (cv::waitKey(1) == 'q') break;
//	}
//
//	cam_top.stop();
//	cam_left.stop();
//	cam_right.stop();
//	cv::destroyAllWindows();
//
//	return 0;
//}