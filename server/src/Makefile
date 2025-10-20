# Server Makefile

CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -MMD -MP -g
INCLUDES := -I../libs
LDFLAGS := -lcurl
BUILD_DIR := build
TARGET := server

# Source files (server.c is the only source file)
SRC := server.c
OBJ := $(BUILD_DIR)/server.o

# Dependencies
DEPS := $(OBJ:.o=.d)

# Ensure the build directory exists before proceeding
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Default target to build the executable
all: $(TARGET)

# Build the target (executable)
$(TARGET): $(OBJ)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJ) -o $@ $(LDFLAGS)
	@echo "Build complete!"

# Rule to compile .c files to .o files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@echo "Source: $<"
	@echo "Output: $@"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run the application
run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "Clean complete!"

# Debug build with extra debugging symbols
debug: CFLAGS += -DDEBUG -O0 -ggdb3
debug: $(TARGET)

# Release build with optimization
release: CFLAGS += -O2 -DNDEBUG
release: $(TARGET)

# Include dependency files
-include $(DEPS)

.PHONY: all clean run debug release
