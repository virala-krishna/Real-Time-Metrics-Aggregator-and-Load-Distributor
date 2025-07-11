# Makefile

CXX = g++
CXXFLAGS = -std=c++11 -pthread
TARGETS = aggregator agent
BUILD_DIR = build

all: $(TARGETS)

agent: agent.cpp
	$(CXX) $(CXXFLAGS) agent.cpp -o $(BUILD_DIR)/agent

aggregator: aggregator.cpp
	$(CXX) $(CXXFLAGS) aggregator.cpp -o $(BUILD_DIR)/aggregator

clean:
	rm -rf $(BUILD_DIR)/*

