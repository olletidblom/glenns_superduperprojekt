# Compiler settings
CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -MMD -MP -g
INCLUDES := -I../../libs
LDFLAGS := -lcurl

# Directory structure
BUILD_DIR := build
TARGET := $(BUILD_DIR)/server

# Source files
SRC := $(wildcard *.c ../../libs/*.c)

# Object and dependency files (in build/)
OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRC)))
DEPS := $(OBJ:.o=.d)

# Ensure the build directory exists
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Default target
all: $(TARGET)

# Link executable
$(TARGET): $(OBJ)
	@echo "Linking $@..."
	$(CC) $(OBJ) -o $@ $(LDFLAGS)
	@echo "Build complete!"

# Compile server source files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile library source files
$(BUILD_DIR)/%.o: ../../libs/%.c | $(BUILD_DIR)
	@echo "Compiling library $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run the program
run: $(TARGET)
	@echo "Running server..."
	./$(TARGET)

# Clean build artifacts
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)
	@echo "Clean complete!"

# Debug build
debug: CFLAGS += -DDEBUG -O0 -ggdb3
debug: $(TARGET)

# Release build
release: CFLAGS += -O2 -DNDEBUG
release: $(TARGET)

# Include dependency files
-include $(DEPS)

.PHONY: all clean run debug release