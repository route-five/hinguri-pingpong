# Makefile for Hinguri Demo Tests
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0

# Test executables
TESTS = test_hinguri_demo test_hinguri_functions test_constants_validation

.PHONY: all clean run-tests test

all: $(TESTS)

test_hinguri_demo: test_hinguri_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test_hinguri_functions: test_hinguri_functions.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test_constants_validation: test_constants_validation.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

run-tests: $(TESTS)
	@echo "Running all Hinguri Demo tests..."
	@echo "\n1. Running comprehensive tests:"
	./test_hinguri_demo
	@echo "\n2. Running function-specific tests:"
	./test_hinguri_functions
	@echo "\n3. Running constants validation:"
	./test_constants_validation

test: run-tests

clean:
	rm -f $(TESTS) *.o