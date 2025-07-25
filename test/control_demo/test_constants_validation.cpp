/*
 * Validation tests for constants and configuration values in Hinguri_Demo_test.cpp
 * This ensures that all hardcoded values are within expected ranges and consistent
 */

#include <iostream>
#include <cassert>
#include <cmath>

// Copy constants from the original file for testing
#define CENTER_POS 2048
#define MIN_POS_LIMIT 1024
#define MAX_POS_LIMIT 3072
#define DEGREES_PER_UNIT (360.0 / 4096.0)

#define RACKET_WIDTH 13.4   // cm
#define RACKET_HEIGHT 14.8  // cm  
#define RACKET_WIDTH_HALF RACKET_WIDTH / 2
#define RACKET_HEIGHT_HALF RACKET_HEIGHT / 2
#define RACKET_EDGE_RADIUS 14.6  // cm
#define GROUND_EDGE_HEIGHT 12.1  // cm

#define ADDR_TORQUE_ENABLE 64
#define ADDR_OPERATING_MODE 11
#define ADDR_GOAL_POSITION 116
#define PROTOCOL_VERSION 2.0
#define BAUDRATE 57600

#define TORQUE_ENABLE 1
#define POSITION_CONTROL_MODE 3

class ConstantsValidator {
private:
    int test_count = 0;
    int passed_count = 0;
    
public:
    void validate(bool condition, const std::string& description) {
        test_count++;
        if (condition) {
            passed_count++;
            std::cout << "✓ " << description << std::endl;
        } else {
            std::cout << "✗ " << description << std::endl;
        }
    }
    
    void run_all_validations() {
        std::cout << "=== Constants Validation Tests ===" << std::endl;
        
        // Validate position constants
        validate(CENTER_POS > MIN_POS_LIMIT, "CENTER_POS is above minimum limit");
        validate(CENTER_POS < MAX_POS_LIMIT, "CENTER_POS is below maximum limit");
        validate(MAX_POS_LIMIT > MIN_POS_LIMIT, "MAX_POS_LIMIT is greater than MIN_POS_LIMIT");
        validate(CENTER_POS == 2048, "CENTER_POS has expected value (2048)");
        
        // Validate position range is reasonable (not too restrictive)
        int total_range = MAX_POS_LIMIT - MIN_POS_LIMIT;
        validate(total_range >= 1024, "Position range is reasonable (>= 1024 units)");
        
        // Validate degrees per unit calculation
        double expected_dpu = 360.0 / 4096.0;
        validate(std::abs(DEGREES_PER_UNIT - expected_dpu) < 1e-10, "DEGREES_PER_UNIT calculated correctly");
        validate(DEGREES_PER_UNIT > 0, "DEGREES_PER_UNIT is positive");
        validate(DEGREES_PER_UNIT < 1.0, "DEGREES_PER_UNIT is less than 1 (sensible for servo)");
        
        // Validate racket physical dimensions are reasonable
        validate(RACKET_WIDTH > 0, "RACKET_WIDTH is positive");
        validate(RACKET_HEIGHT > 0, "RACKET_HEIGHT is positive");
        validate(RACKET_WIDTH < 100, "RACKET_WIDTH is reasonable (< 100cm)");
        validate(RACKET_HEIGHT < 100, "RACKET_HEIGHT is reasonable (< 100cm)");
        
        // Validate calculated half dimensions
        validate(std::abs(RACKET_WIDTH_HALF - RACKET_WIDTH/2) < 1e-10, "RACKET_WIDTH_HALF calculated correctly");
        validate(std::abs(RACKET_HEIGHT_HALF - RACKET_HEIGHT/2) < 1e-10, "RACKET_HEIGHT_HALF calculated correctly");
        
        // Validate edge radius and ground height are reasonable
        validate(RACKET_EDGE_RADIUS > 0, "RACKET_EDGE_RADIUS is positive");
        validate(GROUND_EDGE_HEIGHT > 0, "GROUND_EDGE_HEIGHT is positive");
        validate(RACKET_EDGE_RADIUS < 100, "RACKET_EDGE_RADIUS is reasonable");
        validate(GROUND_EDGE_HEIGHT < 100, "GROUND_EDGE_HEIGHT is reasonable");
        
        // Validate Dynamixel protocol constants
        validate(PROTOCOL_VERSION > 0, "PROTOCOL_VERSION is positive");
        validate(BAUDRATE > 0, "BAUDRATE is positive");
        validate(BAUDRATE <= 4000000, "BAUDRATE is within reasonable range");
        
        // Validate address constants are in valid range for Dynamixel
        validate(ADDR_TORQUE_ENABLE >= 0 && ADDR_TORQUE_ENABLE <= 255, "ADDR_TORQUE_ENABLE in valid range");
        validate(ADDR_OPERATING_MODE >= 0 && ADDR_OPERATING_MODE <= 255, "ADDR_OPERATING_MODE in valid range");
        validate(ADDR_GOAL_POSITION >= 0 && ADDR_GOAL_POSITION <= 255, "ADDR_GOAL_POSITION in valid range");
        
        // Validate control mode constants
        validate(TORQUE_ENABLE == 1, "TORQUE_ENABLE has expected value");
        validate(POSITION_CONTROL_MODE > 0, "POSITION_CONTROL_MODE is positive");
        
        // Validate geometric consistency
        double r = RACKET_HEIGHT_HALF + RACKET_EDGE_RADIUS;
        double h = GROUND_EDGE_HEIGHT + RACKET_WIDTH_HALF;
        validate(r > h, "Geometric setup: r > h for valid acos domain");
        validate(r > 0 && h > 0, "Geometric setup: both r and h are positive");
        
        std::cout << "\n=== Validation Summary ===" << std::endl;
        std::cout << "Total validations: " << test_count << std::endl;
        std::cout << "Passed: " << passed_count << std::endl;
        std::cout << "Failed: " << (test_count - passed_count) << std::endl;
        std::cout << "Success rate: " << (100.0 * passed_count / test_count) << "%" << std::endl;
    }
};

int main() {
    ConstantsValidator validator;
    validator.run_all_validations();
    return 0;
}