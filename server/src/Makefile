# Compiler settings
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -I../libs
LDFLAGS = 

# Directories
SRC_DIR   = .
LIB_DIR   = ../../libs
BUILD_DIR = ../build

# Source files
SRC_FILE = server.c
LIB_FILES = $(LIB_DIR)/TCP.c $(LIB_DIR)/HTTP.c

# Object files
OBJ_FILES = $(BUILD_DIR)/server.o $(BUILD_DIR)/TCP.o $(BUILD_DIR)/HTTP.o

# Output binary
TARGET = $(BUILD_DIR)/server_program

# Default target
all: $(BUILD_DIR) $(TARGET)

# Link all object files
$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)
	@echo "✅ Build complete: $(TARGET)"

# Compile rules
$(BUILD_DIR)/server.o: $(SRC_DIR)/server.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/TCP.o: $(LIB_DIR)/TCP.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/HTTP.o: $(LIB_DIR)/HTTP.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Cleanup
clean:
	rm -rf $(BUILD_DIR)/*
	@echo "🧹 Cleaned up build files."

# Optional: run the program
run: all
	./$(TARGET)