# Compiler settings
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -I../libs
LDFLAGS = 

# Directories
SRC_DIR   = .
LIB_DIR   = ../../libs
BUILD_DIR = build

# Source files
SRC_FILE = main.c
LIB_FILES = $(LIB_DIR)/TCPClient.c $(LIB_DIR)/HTTP.c

# Object files
OBJ_FILES = $(BUILD_DIR)/main.o $(BUILD_DIR)/TCPClient.o $(BUILD_DIR)/HTTP.o $(BUILD_DIR)/smw.o

# Output binary
TARGET = $(BUILD_DIR)/client

# Default target
all: $(BUILD_DIR) $(TARGET)

# Link all object files
$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)
	@echo "✅ Build complete: $(TARGET)"

# Compile rules

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/TCPClient.o: $(LIB_DIR)/TCPClient.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/HTTP.o: $(LIB_DIR)/HTTP.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/smw.o: $(LIB_DIR)/smw.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# Add a debug build target
CFLAGS_DEBUG = $(CFLAGS) -g -fsanitize=address -fno-omit-frame-pointer
LDFLAGS_DEBUG = $(LDFLAGS) -fsanitize=address

debug: CFLAGS := $(CFLAGS_DEBUG)
debug: LDFLAGS := $(LDFLAGS_DEBUG)
debug: clean all

valgrind: all
    valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)


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

# API example
api: $(BUILD_DIR)/api_example.o $(BUILD_DIR)/TCPClient.o $(BUILD_DIR)/HTTP.o $(BUILD_DIR)/API.o $(BUILD_DIR)/smw.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/api_example $(LDFLAGS)
	@echo "✅ API example built successfully!"

$(BUILD_DIR)/api_example.o: api_example.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/API.o: $(LIB_DIR)/API.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run API example
run-api: api
	./$(BUILD_DIR)/api_example

# JSON API example
json-api: $(BUILD_DIR)/json_api_example.o $(BUILD_DIR)/TCPClient.o $(BUILD_DIR)/HTTP.o $(BUILD_DIR)/API.o $(BUILD_DIR)/smw.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/json_api_example $(LDFLAGS)
	@echo "✅ JSON API example built successfully!"

$(BUILD_DIR)/json_api_example.o: json_api_example.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run JSON API example
run-json-api: json-api
	./$(BUILD_DIR)/json_api_example
	./$(BUILD_DIR)/api_example

# Simple API example
simple-api: $(BUILD_DIR)/simple_api_example.o $(BUILD_DIR)/TCPClient.o $(BUILD_DIR)/HTTP.o $(BUILD_DIR)/API.o $(BUILD_DIR)/smw.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/simple_api_example $(LDFLAGS)
	@echo "✅ Simple API example built successfully!"

$(BUILD_DIR)/simple_api_example.o: simple_api_example.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run simple API example
run-simple-api: simple-api
	./$(BUILD_DIR)/simple_api_example