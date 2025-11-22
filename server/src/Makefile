# Compiler settings
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -I../../libs
LDFLAGS = 

# Directories
SRC_DIR   = .
LIB_DIR   = ../../libs
BUILD_DIR = ../build

# Source files
SRC_FILES = main.c TCPServer.c HTTPServer.c HTTPServerConnection.c
LIB_FILES = $(LIB_DIR)/TCPClient.c $(LIB_DIR)/smw.c

# Object files
OBJ_FILES = $(BUILD_DIR)/TCPClient.o $(BUILD_DIR)/TCPServer.o $(BUILD_DIR)/HTTPServer.o  $(BUILD_DIR)/smw.o $(BUILD_DIR)/main.o $(BUILD_DIR)/HTTPServerConnection.o

# Output binary
TARGET = $(BUILD_DIR)/server_program

# Default target
all: $(BUILD_DIR) $(TARGET)

# Add a debug build target
CFLAGS_DEBUG = $(CFLAGS) -g -fsanitize=address -fno-omit-frame-pointer
LDFLAGS_DEBUG = $(LDFLAGS) -fsanitize=address

debug: CFLAGS := $(CFLAGS_DEBUG)
debug: LDFLAGS := $(LDFLAGS_DEBUG)
debug: clean all

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Link all object files
$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)
	@echo "✅ Build complete: $(TARGET)"

# Compile rules


$(BUILD_DIR)/TCPClient.o: $(LIB_DIR)/TCPClient.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/TCPServer.o: $(SRC_DIR)/TCPServer.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/HTTPServer.o: $(SRC_DIR)/HTTPServer.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/HTTPServerConnection.o: $(SRC_DIR)/HTTPServerConnection.c | $(BUILD_DIR)
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