# Compiler settings
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -I../../libs
LDFLAGS = 

# Directories
SRC_DIR   = .
LIB_DIR   = ../../libs
BUILD_DIR = ../build

# Source files
SRC_FILE = server.c main.c
LIB_FILES = $(LIB_DIR)/TCPClient.c $(LIB_DIR)/TCPServer.c $(LIB_DIR)/HTTP.c $(LIB_DIR)/smw.c

# Object files
OBJ_FILES = $(BUILD_DIR)/server.o $(BUILD_DIR)/TCPClient.o $(BUILD_DIR)/TCPServer.o $(BUILD_DIR)/HTTP.o $(BUILD_DIR)/smw.o $(BUILD_DIR)/main.o

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

$(BUILD_DIR)/TCPClient.o: $(LIB_DIR)/TCPClient.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/TCPServer.o: $(LIB_DIR)/TCPServer.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/HTTP.o: $(LIB_DIR)/HTTP.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/smw.o: $(LIB_DIR)/smw.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c | $(BUILD_DIR)
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