//
// Created by 임정훈, Hyunseung Ryu on 25. 7. 27.
//

#define _USE_MATH_DEFINES
#include <opencv2/opencv.hpp>
#include <atomic>
#include <optional>
#include <thread>
#include <iostream>

#include "vision/calibrator.hpp"
#include "vision/camera.hpp"
#include "vision/camera_type.hpp"
#include "vision/predictor.hpp"
#include "vision/tracker.hpp"
// #include "vision/visualizer.hpp"
#include "../utils/constants.hpp"
#include "../utils/draw.hpp"
#include "dynamixel_sdk.h"
#include "CLinear_actu.h"
#include "constants.hpp"

#define IS_REVERSED 1
#define CALIBRATION 0

#define ADDR_TORQUE_ENABLE 64
#define ADDR_OPERATING_MODE 11
#define ADDR_GOAL_POSITION 116
#define PROTOCOL_VERSION 2.0
#define BAUDRATE 57600
#define DEVICENAME "COM4"

#define TORQUE_ENABLE 1
#define POSITION_CONTROL_MODE 3

#define CENTER_POS 2048
#define MIN_POS_LIMIT 1024
#define MAX_POS_LIMIT 3072
#define DEGREES_PER_UNIT (360.0 / 4096.0)  // 약 0.088°

#define RACKET_WIDTH 13.4   // cm
#define RACKET_HEIGHT 14.8  // cm
#define RACKET_WIDTH_HALF RACKET_WIDTH / 2
#define RACKET_HEIGHT_HALF RACKET_HEIGHT / 2
#define RACKET_EDGE_RADIUS 14.6  // cm
#define GROUND_EDGE_HEIGHT 12.1  // cm

#define START_SWING -90; // deg
#define END_SWING 30; // deg

constexpr int TOP_MOTOR_ID = 4;
constexpr int MID_MOTOR_ID = 3;
constexpr int BOT_MOTOR_ID = 2;

double rad_to_deg(double rad) {
    return rad * 180.0 / M_PI;
}

double deg_to_rad(double deg) {
    return deg * M_PI / 180.0;
}

struct BridgePayload {
    double x;
    double z;
    double angle;
    double velocity;
};

std::mutex mutex;
std::optional<BridgePayload> shared_payload;
std::atomic<bool> has_sent{false};

class VisionEnd {
public:
    Camera cam_top = Camera(CameraType::TOP, {0, 1400}, 120);
    Camera cam_left = Camera(CameraType::LEFT, {1, 1400}, 120);
    Camera cam_right = Camera(CameraType::RIGHT, {2, 1400}, 120);
    Tracker tracker_left = Tracker(ORANGE_MIN, ORANGE_MAX);
    Tracker tracker_right = Tracker(ORANGE_MIN, ORANGE_MAX);
    Tracker tracker_top = Tracker(ORANGE_MIN, ORANGE_MAX);
    Calibrator calibrator_top = Calibrator(cam_top);
    Predictor predictor;

    static void callback(
        cv::Mat& frame,
        const Camera& camera,
        Tracker& tracker,
        const std::function<void(const cv::Point2f&)>& set_pt
    ) {
        if (frame.empty()) return;

        tracker.update(frame);

        const auto ret = tracker.get_camera_pos();
        if (ret.has_value()) {
            auto [center, radius] = ret.value();

            cv::circle(frame, center, radius, COLOR_GREEN, -1, cv::LINE_AA);
            set_pt(center);
        }

        Draw::put_text(
            frame,
            std::format("FPS: {:.1f}/{:.1f}", camera.get_fps(), camera.get_prop(cv::CAP_PROP_FPS)),
            {10, 30}
        );
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

        // (2) 탁구공 위치 검출
        cam_top.set_frame_callback([&](cv::Mat& frame) {
            callback(frame, cam_top, tracker_top, [this](const cv::Point2f& pt) {
                predictor.set_point_top(pt);
            });
        });
        cam_left.set_frame_callback([&](cv::Mat& frame) {
            callback(frame, cam_left, tracker_left, [this](const cv::Point2f& pt) {
                predictor.set_point_left(pt);
            });
        });
        cam_right.set_frame_callback([&](cv::Mat& frame) {
            callback(frame, cam_right, tracker_right, [this](const cv::Point2f& pt) {
                predictor.set_point_right(pt);
            });
        });

        cam_top.start();
        cam_left.start();
        cam_right.start();

        cv::Point3f world_pos{-1, -1, -1};
        cv::Point3f predict_pos;
        cv::Point3f real_arrive_pos;
        std::deque<cv::Point3f> orbit;

        while (true) {
            cv::Mat frame_top = cam_top.read(),
                    frame_left = cam_left.read(),
                    frame_right = cam_right.read();

            if (frame_top.empty() || frame_left.empty() || frame_right.empty())
                continue;

            // (3) 삼각측량 및 bird's eye view로 3D 위치 계산
            tracker_top.update(frame_top);
            auto ret = tracker_top.get_camera_pos();
            world_pos = predictor.get_new_world_pos(ret.has_value() ? &ret.value().first : nullptr);

            // 궤적 100개 제한해서 저장
            orbit.push_back(world_pos);
            if (orbit.size() > 100) {
                orbit.pop_front();
            }

            Draw::put_text(
                frame_top,
                Draw::to_string("Pos", world_pos, "cm"),
                {10, 70}
            );
            Draw::put_text(
                frame_left,
                Draw::to_string("Pos", world_pos, "cm"),
                {10, 70}
            );

            if (predictor.get_world_positions_size() >= 2) {
                auto world_speed = predictor.get_world_speed();

                Draw::put_text(
                    frame_left,
                    Draw::to_string("Speed", world_speed, "cm/s"),
                    {10, 110}
                );

                // 네트를 3/4 넘길 때까지만 예측
                if (TABLE_HEIGHT / 4 <= world_pos.y && world_pos.y <= TABLE_HEIGHT) {
                    const auto predict = predictor.get_arrive_pos();
                    if (predict.has_value()) {
                        predict_pos = predict.value();
                    }
                }
                else if (0 <= world_pos.x && world_pos.x <= TABLE_WIDTH && 0 <= world_pos.y && 0 <= world_pos.z && !
                    has_sent.load()) {
                    {
                        std::lock_guard<std::mutex> lock(mutex);
                        BridgePayload input{
                            predict_pos.x - TABLE_WIDTH / 2,
                            predict_pos.z,
                            deg_to_rad(35),
                            0.0
                        };

                        std::cout << "(" << world_pos.x << ", " << world_pos.z << ") 로 호출" << std::endl;

                        shared_payload = input;
                        has_sent = true;
                    }
                }
            }

            // 실제 도착 위치 저장
            if (-5 <= world_pos.y && world_pos.y < 3) {
                real_arrive_pos = world_pos;
            }

            Draw::put_text(
                frame_top,
                Draw::to_string("Predict", predict_pos, "cm"),
                {10, 150}
            );
            Draw::put_text(
                frame_left,
                Draw::to_string("Predict", predict_pos, "cm"),
                {10, 150}
            );

            Draw::put_text(
                frame_top,
                Draw::to_string("Arrive", real_arrive_pos, "cm"),
                {10, 190}
            );
            Draw::put_text(
                frame_left,
                Draw::to_string("Arrive", real_arrive_pos, "cm"),
                {10, 190}
            );

            // 궤적 그리기
            for (const auto& pos : orbit) {
                cv::Point2f predict_left = predictor.pos_3d_to_2d(pos, CameraType::LEFT);
                cv::Point2f predict_right = predictor.pos_3d_to_2d(pos, CameraType::RIGHT);
                cv::Point2f predict_top = predictor.pos_3d_to_2d(pos, CameraType::TOP);
                cv::circle(frame_left, predict_left, 3, COLOR_MAGENTA, -1, cv::LINE_AA);
                cv::circle(frame_right, predict_right, 3, COLOR_MAGENTA, -1, cv::LINE_AA);
                cv::circle(frame_top, predict_top, 3, COLOR_MAGENTA, -1, cv::LINE_AA);
            }

            // 예상 도착 위치 그리기
            const auto predict_left = predictor.pos_3d_to_2d(predict_pos, CameraType::LEFT);
            const auto predict_right = predictor.pos_3d_to_2d(predict_pos, CameraType::RIGHT);
            const auto predict_top = predictor.pos_3d_to_2d(predict_pos, CameraType::TOP);
            cv::circle(frame_left, predict_left, 10, COLOR_BLUE, -1, cv::LINE_AA);
            cv::circle(frame_right, predict_right, 10, COLOR_BLUE, -1, cv::LINE_AA);
            cv::circle(frame_top, predict_top, 10, COLOR_BLUE, -1, cv::LINE_AA);

            // 실제 도착 위치 그리기
            const auto real_left = predictor.pos_3d_to_2d(real_arrive_pos, CameraType::LEFT);
            const auto real_right = predictor.pos_3d_to_2d(real_arrive_pos, CameraType::RIGHT);
            const auto real_top = predictor.pos_3d_to_2d(real_arrive_pos, CameraType::TOP);
            cv::circle(frame_left, real_left, 10, COLOR_RED, -1, cv::LINE_AA);
            cv::circle(frame_right, real_right, 10, COLOR_RED, -1, cv::LINE_AA);
            cv::circle(frame_top, real_top, 10, COLOR_RED, -1, cv::LINE_AA);

            cv::imshow("Left", frame_left);
            cv::imshow("Right", frame_right);
            cv::imshow("Top", frame_top);

            if (cv::waitKey(1) == 'q') break;
        }

        cam_top.stop();
        cam_left.stop();
        cam_right.stop();
        cv::destroyAllWindows();
    }
};

// Shared handlers for all Dynamixel actuators
static dynamixel::PortHandler* sharedPortHandler =
    dynamixel::PortHandler::getPortHandler(DEVICENAME);
static dynamixel::PacketHandler* sharedPacketHandler =
    dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
static bool sharedPortInitialized = false;

class BulkDynamixelActuator {
private:
    dynamixel::PortHandler* portHandler;
    dynamixel::PacketHandler* packetHandler;
    std::vector<int> ids;

public:
    explicit BulkDynamixelActuator(const std::vector<int>& motor_ids) : ids{motor_ids} {
        // Use shared handlers
        portHandler = sharedPortHandler;
        packetHandler = sharedPacketHandler;
    }

    ~BulkDynamixelActuator() {
        for (const auto& id : ids) {
            close(id);
        }
    }

    [[nodiscard]] bool initialize() const {
        if (!sharedPortInitialized) {
            if (!portHandler->openPort()) {
                std::cerr << "Failed to open port" << std::endl;
                return false;
            }
            if (!portHandler->setBaudRate(BAUDRATE)) {
                std::cerr << "Failed to set baudrate" << std::endl;
                return false;
            }
            sharedPortInitialized = true;
        }

        for (const auto& id : ids) {
            uint8_t dxl_error = 0;

            // Ensure torque is disabled before changing operating mode
            packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, 0, &dxl_error);

            int result =
                packetHandler->write1ByteTxRx(portHandler, id, ADDR_OPERATING_MODE,
                                              POSITION_CONTROL_MODE, &dxl_error);
            if (result != COMM_SUCCESS || dxl_error) {
                std::cerr << "Failed to set operating mode for motor " << id << std::endl;
                return false;
            }

            result = packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE,
                                                   TORQUE_ENABLE, &dxl_error);
            if (result != COMM_SUCCESS || dxl_error) {
                std::cerr << "Failed to enable torque for motor " << id << std::endl;
                return false;
            }
        }

        std::cout << "All motors initialized successfully" << std::endl;
        return true;
    }

    void bulk_move_by_degrees(const std::vector<double>& degree_offsets) const {
        dynamixel::GroupBulkWrite groupBulkWrite(portHandler, packetHandler);
        if (degree_offsets.size() != ids.size()) {
            std::cerr << "Degree offsets size does not match motor IDs size" << std::endl;
            return;
        }

        std::vector<int> target_positions;
        for (size_t i = 0; i < ids.size(); ++i) {
            int offset_units = static_cast<int>(degree_offsets[i] / DEGREES_PER_UNIT);
            int target_position = CENTER_POS + offset_units;

            // 안전 범위 제한
            if (target_position < MIN_POS_LIMIT || target_position > MAX_POS_LIMIT) {
                std::cerr << "Movement out of range (" << target_position << "). Ignored."
                    << std::endl;
                return;
            }
            target_positions.push_back(target_position);
        }

        uint8_t dxl_error = 0;
        bool dxl_addparam_result = false;
        for (size_t i = 0; i < ids.size(); ++i) {
            // id, start_address, data_length, *data
            dxl_addparam_result = groupBulkWrite.addParam(
                ids[i], ADDR_GOAL_POSITION, 4,
                reinterpret_cast<uint8_t*>(&target_positions[i])
            );
            if (!dxl_addparam_result) {
                std::cerr << "Failed to add parameter for motor " << ids[i] << std::endl;
                return;
            }
        }

        bool dxl_comm_result = groupBulkWrite.txPacket();
        if (dxl_comm_result != COMM_SUCCESS) {
            std::cerr << "Failed to write bulk data" << std::endl;
            return;
        }

        groupBulkWrite.clearParam();
    }

    void close(int id) const {
        uint8_t dxl_error = 0;
        packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, 0, &dxl_error);
        std::cout << "Torque disabled for motor " << id << std::endl;
    }
};

class ControlEnd {
public:
    ControlEnd()
        : actuators({TOP_MOTOR_ID, MID_MOTOR_ID, BOT_MOTOR_ID}) {
    }

    ~ControlEnd() {
        shutdown();
    }

    void run() {
        if (!actuators.initialize()) {
            std::cerr << "Failed to initialize actuators" << std::endl;
            return;
        }
        while (true) {
            if (!has_sent.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            BridgePayload input;
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (!shared_payload.has_value()) continue;
                input = shared_payload.value();
                shared_payload.reset();
                std::cout << "(" << input.x << ", " << input.z << ") 로 받음" << std::endl;
                has_sent = false;
            }
            if (input.x == -1000) break;
            execute(input);
        }
        shutdown();
    }

private:
    BulkDynamixelActuator actuators;
    LinearActuator linearActuator;

    void execute(const BridgePayload& input) {
        double p = 1;
        double r = RACKET_HEIGHT_HALF + RACKET_EDGE_RADIUS;
        double h = GROUND_EDGE_HEIGHT + RACKET_WIDTH_HALF;
        double theta = std::acos((input.z - h) / r);
        double x = input.x - r * std::sin(theta);
        if (input.x < 0) p = -1;
        double top_target = rad_to_deg(-input.angle * p);
        double mid_target = rad_to_deg(theta * p);
        double bot_target = p * START_SWING;
        actuators.bulk_move_by_degrees({top_target, mid_target, bot_target});
        linearActuator.move_actu(IS_REVERSED ? -x : x);
        bot_target = p * END_SWING;
        actuators.bulk_move_by_degrees({bot_target, bot_target, bot_target});
    }

    void shutdown() {
        actuators.bulk_move_by_degrees({0, 0, 0});
        linearActuator.move_actu(0);
        sharedPortHandler->closePort();
    }
};

int main() {
    VisionEnd vision_end;
    std::thread vision_thread(&VisionEnd::run, &vision_end);

    ControlEnd control_end;
    std::thread control_thread(&ControlEnd::run, &control_end);

    vision_thread.join();
    control_thread.join();

    return 0;
}