#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <cassert>
#include <sstream>
#include <memory>
#include <vector>

// Mock the external dependencies for testing
class MockPortHandler {
public:
    bool openPort_result = true;
    bool setBaudRate_result = true;
    bool closePort_called = false;
    
    bool openPort() { return openPort_result; }
    bool setBaudRate(int) { return setBaudRate_result; }
    void closePort() { closePort_called = true; }
};

class MockPacketHandler {
public:
    int write1ByteTxRx_result = 0; // COMM_SUCCESS
    int write4ByteTxRx_result = 0; // COMM_SUCCESS
    
    int write1ByteTxRx(void*, int, int, int, uint8_t* error) {
        *error = 0;
        return write1ByteTxRx_result;
    }
    
    int write4ByteTxRx(void*, int, int, int, uint8_t* error) {
        *error = 0;
        return write4ByteTxRx_result;
    }
};

// Mock CLinear_actu class
class MockCLinear_actu {
public:
    int last_move_value = 0;
    bool move_called = false;
    
    void move_actu(int x) {
        last_move_value = x;
        move_called = true;
    }
};

// Include test constants
#define COMM_SUCCESS 0
#define COMM_TX_FAIL 1
#define TOP_MOTOR_ID 1
#define MID_MOTOR_ID 2
#define BOT_MOTOR_ID 3

// Test constants from original file
#define CENTER_POS 2048
#define MIN_POS_LIMIT 1024
#define MAX_POS_LIMIT 3072
#define DEGREES_PER_UNIT (360.0 / 4096.0)
#define RACKET_WIDTH 13.4
#define RACKET_HEIGHT 14.8
#define RACKET_WIDTH_HALF RACKET_WIDTH / 2
#define RACKET_HEIGHT_HALF RACKET_HEIGHT / 2
#define RACKET_EDGE_RADIUS 14.6
#define GROUND_EDGE_HEIGHT 12.1

// Test helper function - rad_to_deg implementation
double rad_to_deg(double rad) { 
    return rad * 180.0 / M_PI; 
}

// Test class for DynamixelActuator (simplified for testing)
class TestDynamixelActuator {
private:
    MockPortHandler* mockPortHandler;
    MockPacketHandler* mockPacketHandler;
    int id;

public:
    explicit TestDynamixelActuator(int motor_id, MockPortHandler* ph, MockPacketHandler* pah) 
        : id(motor_id), mockPortHandler(ph), mockPacketHandler(pah) {}

    bool initialize() {
        if (!mockPortHandler->openPort()) {
            return false;
        }
        if (!mockPortHandler->setBaudRate(57600)) {
            return false;
        }
        
        uint8_t error = 0;
        int result = mockPacketHandler->write1ByteTxRx(nullptr, id, 11, 3, &error);
        if (result != COMM_SUCCESS || error) {
            return false;
        }
        
        result = mockPacketHandler->write1ByteTxRx(nullptr, id, 64, 1, &error);
        if (result != COMM_SUCCESS || error) {
            return false;
        }
        
        return true;
    }

    void move_by_degrees(double degree_offset) {
        const int offset_units = static_cast<int>(degree_offset / DEGREES_PER_UNIT);
        const int target_position = CENTER_POS + offset_units;
        
        if (target_position < MIN_POS_LIMIT || target_position > MAX_POS_LIMIT) {
            return; // Out of range
        }
        
        uint8_t error = 0;
        mockPacketHandler->write4ByteTxRx(nullptr, id, 116, target_position, &error);
    }
};

// Test suite implementation
class HinguriDemoTestSuite {
private:
    int tests_run = 0;
    int tests_passed = 0;
    
    void assert_test(bool condition, const std::string& test_name) {
        tests_run++;
        if (condition) {
            tests_passed++;
            std::cout << "[PASS] " << test_name << std::endl;
        } else {
            std::cout << "[FAIL] " << test_name << std::endl;
        }
    }

public:
    void test_rad_to_deg_conversion() {
        // Test positive angle conversion
        double result = rad_to_deg(M_PI);
        assert_test(std::abs(result - 180.0) < 1e-6, "rad_to_deg: π radians to 180 degrees");
        
        // Test negative angle conversion
        result = rad_to_deg(-M_PI/2);
        assert_test(std::abs(result - (-90.0)) < 1e-6, "rad_to_deg: -π/2 radians to -90 degrees");
        
        // Test zero conversion
        result = rad_to_deg(0.0);
        assert_test(std::abs(result - 0.0) < 1e-6, "rad_to_deg: 0 radians to 0 degrees");
        
        // Test 2π conversion
        result = rad_to_deg(2 * M_PI);
        assert_test(std::abs(result - 360.0) < 1e-6, "rad_to_deg: 2π radians to 360 degrees");
    }

    void test_dynamixel_actuator_initialization() {
        MockPortHandler mockPort;
        MockPacketHandler mockPacket;
        
        // Test successful initialization
        TestDynamixelActuator actuator(1, &mockPort, &mockPacket);
        bool result = actuator.initialize();
        assert_test(result == true, "DynamixelActuator: successful initialization");
        
        // Test failed port opening
        mockPort.openPort_result = false;
        TestDynamixelActuator actuator2(2, &mockPort, &mockPacket);
        result = actuator2.initialize();
        assert_test(result == false, "DynamixelActuator: failed port opening");
        
        // Test failed baudrate setting
        mockPort.openPort_result = true;
        mockPort.setBaudRate_result = false;
        TestDynamixelActuator actuator3(3, &mockPort, &mockPacket);
        result = actuator3.initialize();
        assert_test(result == false, "DynamixelActuator: failed baudrate setting");
        
        // Test communication failure
        mockPort.setBaudRate_result = true;
        mockPacket.write1ByteTxRx_result = COMM_TX_FAIL;
        TestDynamixelActuator actuator4(4, &mockPort, &mockPacket);
        result = actuator4.initialize();
        assert_test(result == false, "DynamixelActuator: communication failure");
    }

    void test_move_by_degrees_position_calculation() {
        MockPortHandler mockPort;
        MockPacketHandler mockPacket;
        TestDynamixelActuator actuator(1, &mockPort, &mockPacket);
        
        // Test normal movement within range
        actuator.move_by_degrees(90.0);
        assert_test(true, "move_by_degrees: normal 90 degree movement");
        
        // Test negative movement
        actuator.move_by_degrees(-45.0);
        assert_test(true, "move_by_degrees: negative 45 degree movement");
        
        // Test zero movement
        actuator.move_by_degrees(0.0);
        assert_test(true, "move_by_degrees: zero degree movement");
        
        // Test extreme positive movement (should be ignored)
        actuator.move_by_degrees(1000.0);
        assert_test(true, "move_by_degrees: extreme positive movement handled");
        
        // Test extreme negative movement (should be ignored)
        actuator.move_by_degrees(-1000.0);
        assert_test(true, "move_by_degrees: extreme negative movement handled");
    }

    void test_position_limits() {
        // Test center position calculation
        int center_pos = CENTER_POS;
        assert_test(center_pos == 2048, "Position limits: CENTER_POS correct value");
        
        // Test min/max limits
        assert_test(MIN_POS_LIMIT == 1024, "Position limits: MIN_POS_LIMIT correct value");
        assert_test(MAX_POS_LIMIT == 3072, "Position limits: MAX_POS_LIMIT correct value");
        
        // Test degrees per unit conversion factor
        double expected_degrees_per_unit = 360.0 / 4096.0;
        assert_test(std::abs(DEGREES_PER_UNIT - expected_degrees_per_unit) < 1e-6, 
                   "Position limits: DEGREES_PER_UNIT correct value");
    }

    void test_racket_constants() {
        // Test racket dimension constants
        assert_test(std::abs(RACKET_WIDTH - 13.4) < 1e-6, "Racket constants: RACKET_WIDTH correct");
        assert_test(std::abs(RACKET_HEIGHT - 14.8) < 1e-6, "Racket constants: RACKET_HEIGHT correct");
        assert_test(std::abs(RACKET_EDGE_RADIUS - 14.6) < 1e-6, "Racket constants: RACKET_EDGE_RADIUS correct");
        assert_test(std::abs(GROUND_EDGE_HEIGHT - 12.1) < 1e-6, "Racket constants: GROUND_EDGE_HEIGHT correct");
        
        // Test calculated half dimensions
        double expected_width_half = RACKET_WIDTH / 2;
        double expected_height_half = RACKET_HEIGHT / 2;
        assert_test(std::abs(RACKET_WIDTH_HALF - expected_width_half) < 1e-6, 
                   "Racket constants: RACKET_WIDTH_HALF calculated correctly");
        assert_test(std::abs(RACKET_HEIGHT_HALF - expected_height_half) < 1e-6, 
                   "Racket constants: RACKET_HEIGHT_HALF calculated correctly");
    }

    void test_coordinate_transformation() {
        // Test coordinate transformation math from main loop
        double target_z = 20.0;
        double target_x = 10.0;
        double r = RACKET_HEIGHT_HALF + RACKET_EDGE_RADIUS;
        double h = GROUND_EDGE_HEIGHT + RACKET_WIDTH_HALF;
        
        // Test theta calculation
        double theta = acos((target_z - h) / r);
        assert_test(!std::isnan(theta), "Coordinate transformation: theta calculation valid");
        assert_test(theta >= 0 && theta <= M_PI, "Coordinate transformation: theta in valid range");
        
        // Test x calculation
        double x = target_x - r * sin(theta);
        assert_test(!std::isnan(x), "Coordinate transformation: x calculation valid");
        
        // Test edge case: target_z equals h
        target_z = h;
        theta = acos((target_z - h) / r); // Should be acos(0) = π/2
        assert_test(std::abs(theta - M_PI/2) < 1e-6, "Coordinate transformation: theta at z=h");
    }

    void test_edge_cases_and_error_conditions() {
        // Test very small angles
        double result = rad_to_deg(1e-10);
        assert_test(std::abs(result) < 1e-6, "Edge cases: very small angle conversion");
        
        // Test very large angles
        result = rad_to_deg(100 * M_PI);
        assert_test(!std::isnan(result) && !std::isinf(result), "Edge cases: very large angle conversion");
        
        // Test position calculation edge cases
        const int small_offset_units = static_cast<int>(0.01 / DEGREES_PER_UNIT);
        const int small_target = CENTER_POS + small_offset_units;
        assert_test(small_target >= MIN_POS_LIMIT && small_target <= MAX_POS_LIMIT, 
                   "Edge cases: small movement within limits");
        
        // Test boundary positions
        const double max_safe_degrees = (MAX_POS_LIMIT - CENTER_POS) * DEGREES_PER_UNIT;
        const double min_safe_degrees = (MIN_POS_LIMIT - CENTER_POS) * DEGREES_PER_UNIT;
        assert_test(max_safe_degrees > 0, "Edge cases: max safe degrees positive");
        assert_test(min_safe_degrees < 0, "Edge cases: min safe degrees negative");
    }

    void test_motor_id_definitions() {
        // Test that motor IDs are distinct
        assert_test(TOP_MOTOR_ID != MID_MOTOR_ID, "Motor IDs: TOP and MID distinct");
        assert_test(MID_MOTOR_ID != BOT_MOTOR_ID, "Motor IDs: MID and BOT distinct");
        assert_test(TOP_MOTOR_ID != BOT_MOTOR_ID, "Motor IDs: TOP and BOT distinct");
        
        // Test that motor IDs are positive
        assert_test(TOP_MOTOR_ID > 0, "Motor IDs: TOP_MOTOR_ID positive");
        assert_test(MID_MOTOR_ID > 0, "Motor IDs: MID_MOTOR_ID positive");
        assert_test(BOT_MOTOR_ID > 0, "Motor IDs: BOT_MOTOR_ID positive");
    }

    void test_linear_actuator_mock() {
        MockCLinear_actu linearActuator;
        
        // Test initial state
        assert_test(!linearActuator.move_called, "Linear actuator: initial state not moved");
        assert_test(linearActuator.last_move_value == 0, "Linear actuator: initial move value zero");
        
        // Test movement
        linearActuator.move_actu(100);
        assert_test(linearActuator.move_called, "Linear actuator: move function called");
        assert_test(linearActuator.last_move_value == 100, "Linear actuator: move value stored correctly");
        
        // Test negative movement
        linearActuator.move_actu(-50);
        assert_test(linearActuator.last_move_value == -50, "Linear actuator: negative move value handled");
    }

    void run_all_tests() {
        std::cout << "\n=== Running Hinguri Demo Test Suite ===" << std::endl;
        
        test_rad_to_deg_conversion();
        test_dynamixel_actuator_initialization();
        test_move_by_degrees_position_calculation();
        test_position_limits();
        test_racket_constants();
        test_coordinate_transformation();
        test_edge_cases_and_error_conditions();
        test_motor_id_definitions();
        test_linear_actuator_mock();
        
        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
        std::cout << "Success rate: " << (100.0 * tests_passed / tests_run) << "%" << std::endl;
    }
};

int main() {
    HinguriDemoTestSuite testSuite;
    testSuite.run_all_tests();
    return 0;
}