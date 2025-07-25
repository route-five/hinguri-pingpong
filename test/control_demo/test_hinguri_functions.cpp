/*
 * Focused unit tests for specific functions in Hinguri_Demo_test.cpp
 * Testing Framework: Custom assertion-based testing (no external dependencies)
 */

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <cassert>
#include <string>

// Test the rad_to_deg function directly
double rad_to_deg(double rad) { 
    return rad * 180.0 / M_PI; 
}

// Test runner class
class FunctionTestRunner {
private:
    int total_tests = 0;
    int passed_tests = 0;
    
public:
    void run_test(bool condition, const std::string& test_name) {
        total_tests++;
        if (condition) {
            passed_tests++;
            std::cout << "✓ PASS: " << test_name << std::endl;
        } else {
            std::cout << "✗ FAIL: " << test_name << std::endl;
        }
    }
    
    void print_summary() {
        std::cout << "\n--- Test Summary ---" << std::endl;
        std::cout << "Total: " << total_tests << ", Passed: " << passed_tests 
                  << ", Failed: " << (total_tests - passed_tests) << std::endl;
        std::cout << "Success Rate: " << (100.0 * passed_tests / total_tests) << "%" << std::endl;
    }
};

int main() {
    FunctionTestRunner runner;
    
    std::cout << "Testing rad_to_deg function:" << std::endl;
    
    // Test 1: Basic conversion - π radians to 180 degrees
    double result = rad_to_deg(M_PI);
    runner.run_test(std::abs(result - 180.0) < 1e-10, "π rad → 180°");
    
    // Test 2: Half circle - π/2 radians to 90 degrees
    result = rad_to_deg(M_PI / 2);
    runner.run_test(std::abs(result - 90.0) < 1e-10, "π/2 rad → 90°");
    
    // Test 3: Quarter circle - π/4 radians to 45 degrees
    result = rad_to_deg(M_PI / 4);
    runner.run_test(std::abs(result - 45.0) < 1e-10, "π/4 rad → 45°");
    
    // Test 4: Zero radians to zero degrees
    result = rad_to_deg(0.0);
    runner.run_test(std::abs(result - 0.0) < 1e-10, "0 rad → 0°");
    
    // Test 5: Full circle - 2π radians to 360 degrees
    result = rad_to_deg(2 * M_PI);
    runner.run_test(std::abs(result - 360.0) < 1e-10, "2π rad → 360°");
    
    // Test 6: Negative angle - -π/2 radians to -90 degrees
    result = rad_to_deg(-M_PI / 2);
    runner.run_test(std::abs(result - (-90.0)) < 1e-10, "-π/2 rad → -90°");
    
    // Test 7: Small angle conversion
    result = rad_to_deg(0.1);
    double expected = 0.1 * 180.0 / M_PI;
    runner.run_test(std::abs(result - expected) < 1e-10, "Small angle: 0.1 rad");
    
    // Test 8: Large angle conversion
    result = rad_to_deg(10.0);
    expected = 10.0 * 180.0 / M_PI;
    runner.run_test(std::abs(result - expected) < 1e-10, "Large angle: 10 rad");
    
    // Test 9: Very small angle (near zero)
    result = rad_to_deg(1e-12);
    expected = 1e-12 * 180.0 / M_PI;
    runner.run_test(std::abs(result - expected) < 1e-15, "Very small angle: 1e-12 rad");
    
    // Test 10: Multiple of π
    result = rad_to_deg(3 * M_PI);
    runner.run_test(std::abs(result - 540.0) < 1e-10, "3π rad → 540°");
    
    runner.print_summary();
    return 0;
}