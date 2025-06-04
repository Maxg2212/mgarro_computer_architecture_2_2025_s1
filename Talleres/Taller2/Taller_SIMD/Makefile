CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

SRC_DIR = src
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(BUILD_DIR)/char_count_serial $(BUILD_DIR)/char_count_simd

$(BUILD_DIR)/char_count_serial: $(SRC_DIR)/char_count_serial.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/char_count_simd: $(SRC_DIR)/char_count_simd.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) -msse2 -o $@ $^

clean:
	rm -f $(BUILD_DIR)/*
