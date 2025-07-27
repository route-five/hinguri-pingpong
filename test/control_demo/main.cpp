//
// Created by 임정훈 on 25. 7. 27.
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


struct ControlInput {
    double x;
    double z;
    double angle;
    double velocity;
};


std::mutex control_mutex;
std::optional<ControlInput> shared_control_input;
std::atomic<bool> new_control_input{ false };

void callback(
    cv::Mat& frame,
    const std::function<void(cv::Point2f&)>& set_pt,
    const Camera& camera,
    Calibrator& calibrator
) {
    if (frame.empty()) return;

    calibrator.undistort(frame, frame, false);

    Tracker tracker{ frame };
    tracker.set_color_mask(ORANGE_MIN, ORANGE_MAX);

    const std::vector<Contour> contours = tracker.find_contours();
    const auto most_contour = tracker.most_circular_contour(contours);

    if (most_contour.has_value() && !most_contour->empty()) {
        auto [pt, radius] = most_contour->min_enclosing_circle();

        if (radius > RADIUS_MIN) {
            cv::circle(frame, pt, radius, COLOR_GREEN, -1, cv::LINE_AA);
            set_pt(pt);
        }
    }

    std::string fps_text = std::format("FPS: {:.1f}/{:.1f}", camera.get_fps(), camera.get_prop(cv::CAP_PROP_FPS));
    cv::putText(frame, fps_text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, COLOR_BLACK, 2, cv::LINE_AA);
}

void vision_end() {
    // (2) 3대의 카메라 타임라인 동기화
    // TODO: 카메라 동기화 로직 구현 필요 - 구현 거의 완료 at PONG#60

    Camera cam_left({ 0, 1400 }, { 1280, 720 }, 120);
    Camera cam_right({ 1, 1400 }, { 1280, 720 }, 120);

    if (!cam_left.is_opened() || !cam_right.is_opened()) {
        const std::string message = std::format(
            "Failed to open camera: left={}, right={}",
            cam_left.is_opened() ? "true" : "false",
            cam_right.is_opened() ? "true" : "false"
        );
        std::cerr << message << std::endl;
        return;
    }

    Predictor predictor;
    Calibrator calibrator_left(CameraType::LEFT, cam_left.get_image_size());
    Calibrator calibrator_right(CameraType::RIGHT, cam_right.get_image_size());

    // (3) 탁구공 센터 검출 (예: blob)
    // TODO: 공 검출 로직 구현 필요 - 배경 제거, blob?
    cam_left.set_frame_callback([&predictor, &cam_left, &calibrator_left](cv::Mat& frame) {
        callback(frame, [&predictor](const cv::Point2f& pt) {
            predictor.set_point_left(pt);
            }, cam_left, calibrator_left);
        });
    cam_right.set_frame_callback([&predictor, &cam_right, &calibrator_right](cv::Mat& frame) {
        callback(frame, [&predictor](const cv::Point2f& pt) {
            predictor.set_point_right(pt);
            }, cam_right, calibrator_right);
        });

    cam_left.start();
    cam_right.start();

    std::vector<cv::Point3f> world_positions;

    while (true) {
        cv::Mat frame_left = cam_left.read(), frame_right = cam_right.read();
        if (frame_left.empty() || frame_right.empty())
            continue;

        cv::Mat concatenated;
        cv::hconcat(frame_left, frame_right, concatenated);

        // (4) 3D 위치 삼각측량
        auto world_pos = predictor.get_world_pos();

        static bool has_sent_command = false;

        if (!has_sent_command && world_pos.y < 3 * TABLE_HEIGHT / 4 && 0 < world_pos.x && world_pos.x < TABLE_WIDTH && 0 < world_pos.y && world_pos.y < TABLE_HEIGHT && world_pos.z > 0) {
            {
                std::lock_guard<std::mutex> lock(control_mutex);
                auto z = 30;
                ControlInput input{
                    world_pos.x - TABLE_WIDTH / 2,
                    z, // TODO: 추후 회귀 필요
                    0.0, // TODO: 추후 예측각도 계산 필요
                    0.0 // TODO: 추후 속도 계산 필요
                };

                std::cout << "(" << world_pos.x << ", " << z << ") 로 호출" << std::endl;

                shared_control_input = input;
                new_control_input = true;
            }

            has_sent_command = true;
        }
        // automatic reset when ball enters half court
        if (has_sent_command && world_pos.y > TABLE_HEIGHT / 2) {
            std::cout << '\a' << std::endl;
            has_sent_command = false;
        }

        // (5) Top 카메라와 비교해서 결과 정밀하게 비교
        // TODO: Top 카메라와 비교하는 로직 구현 필요

        // (6) Kalman filter 등의 후처리로 결과 보정
        // TODO: Kalman filter 적용 로직 구현 필요

        world_positions.push_back(world_pos);

        std::string world_pos_text = std::format(
            "World Position: ({:.2f}, {:.2f}, {:.2f})",
            world_pos.x, world_pos.y, world_pos.z
        );

        cv::putText(
            concatenated, world_pos_text, cv::Point(10, 70),
            cv::FONT_HERSHEY_SIMPLEX, 1, COLOR_BLACK, 2, cv::LINE_AA
        );

        cv::imshow("Left / Right", concatenated);

        int key = cv::waitKey(1);
        if (key == 'q') {
            // signal controller to shutdown
            {
                std::lock_guard<std::mutex> lock(control_mutex);
                shared_control_input = ControlInput{ -1000, 0.0, 0.0, 0.0 };
                new_control_input = true;
            }
            break;
        }
        else if (key == ' ') {
            has_sent_command = false;
            std::cout << "reset chance" << std::endl;
        }
    }

    cam_left.stop();
    cam_right.stop();
}

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
    explicit BulkDynamixelActuator(const std::vector<int>& motor_ids) : ids{ motor_ids } {
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

double rad_to_deg(double rad) { return rad * 180.0 / M_PI; }

class RobotController {
public:
    RobotController()
        : actuators({ TOP_MOTOR_ID, MID_MOTOR_ID, BOT_MOTOR_ID })
    {}

    void run() {
        if (!actuators.initialize()) {
            std::cerr << "Failed to initialize actuators" << std::endl;
            return;
        }
        while (true) {
            if (!new_control_input.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            ControlInput input;
            {
                std::lock_guard<std::mutex> lock(control_mutex);
                if (!shared_control_input.has_value()) continue;
                input = *shared_control_input;
                shared_control_input.reset();
                new_control_input = false;
            }
            if (input.x == -1000) break;
            execute(input);
        }
        shutdown();
    }

private:
    BulkDynamixelActuator actuators;
    LinearActuator linearActuator;

    void execute(const ControlInput& input) {
        double p = 1;
        double r = RACKET_HEIGHT_HALF + RACKET_EDGE_RADIUS;
        double h = GROUND_EDGE_HEIGHT + RACKET_WIDTH_HALF;
        double theta = std::acos((input.z - h) / r);
        double x = input.x - r * std::sin(theta);
        if (input.x < 0) p = -1;
        double top_target = rad_to_deg(-input.angle * p);
        double mid_target = rad_to_deg(theta * p);
        double bot_target = p * START_SWING;
        actuators.bulk_move_by_degrees({ top_target, mid_target, bot_target });
        linearActuator.move_actu(IS_REVERSED ? -x : x);
        bot_target = p * END_SWING;
        actuators.bulk_move_by_degrees({ bot_target, bot_target, bot_target });
    }

    void shutdown() {
        actuators.bulk_move_by_degrees({ 0, 0, 0 });
        linearActuator.move_actu(0);
        sharedPortHandler->closePort();
    }
};

int main() {
    std::thread vision_thread(vision_end);
    RobotController controller;
    std::thread control_thread(&RobotController::run, &controller);

    vision_thread.join();
    control_thread.join();

    return 0;
}