# C++-client/Makefile
# TODO: Make adjustmenets similar to makefiles in server/src and client-c/src
CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -MMD -MP -g
INCLUDES := -I../libs
LDFLAGS := -lcurl
SRC_DIR := src
LIBS_DIR := ../libs
BUILD_DIR := build
TARGET := c++-client

# Source files (now under src/)
SRC := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(LIBS_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Dependencies
DEPS := $(OBJ:.o=.d)

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJ)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)
	@echo "Build complete!"

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

	
$(BUILD_DIR)/%.o: $(LIBS_DIR)/%.cpp  # Compile utility files
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling utility $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Run the application
run: $(TARGET)
	@echo "Running the C++ client..."
	./$(TARGET)

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "Clean complete!"

# Debug build with extra debugging symbols
debug: CXXFLAGS += -DDEBUG -O0 -ggdb3
debug: $(TARGET)

# Release build with optimization
release: CXXFLAGS += -O2 -DNDEBUG
release: $(TARGET)

# Include dependency files
-include $(DEPS)

.PHONY: all clean run debug release
