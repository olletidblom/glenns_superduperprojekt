# Client-C Makefile

CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -MMD -MP -g # -Werror
INCLUDES := -I../libs
SRC_DIR := .
BUILD_DIR := build
TARGET := c-client

# Source and object files
SRC := $(SRC_DIR)/main.c
OBJ := $(BUILD_DIR)/main.o

# Dependencies
DEPS := $(OBJ:.o=.d)

# Default target
all: $(BUILD_DIR) $(TARGET)

# Build the target
$(TARGET): $(OBJ)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJ) -o $(TARGET) -lcurl
	@echo "Build complete!"

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Ensure the build directory exists before compiling
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)


# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "Clean complete!"

# Include dependencies
-include $(DEPS)

.PHONY: all clean
