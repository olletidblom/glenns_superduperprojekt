CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 
LDFLAGS := -lcurl

BUILD := build
DEBUG_BUILD := build_debug

TARGET := client_cpp
DEBUG_TARGET := client_cpp_debug

SRC := $(shell find . -name "*.cpp")

OBJ := $(patsubst %.cpp, $(BUILD)/%.o, $(SRC))
DEBUG_OBJ := $(patsubst %.cpp, $(DEBUG_BUILD)/%.o, $(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug: CXXFLAGS += -g -O0 -DDEBUG
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(DEBUG_BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD) $(DEBUG_BUILD) $(TARGET) $(DEBUG_TARGET)

.PHONY: all debug clean