# ===== Configuration =====
CC       := afl-clang-fast
CFLAGS   := -O1 -g -Wall -Wextra -Wno-unused-parameter \
            -fsanitize=address,undefined \
            -fno-omit-frame-pointer
LDFLAGS  := -fsanitize=address,undefined

TARGET   := fuzz_http

# Your sources (adjust as needed)
SRCS := \
    afl_http_flow.c \
	HTTPServerConnection_fuzz.c

OBJS := $(SRCS:.c=.o)

# ===== Build rules =====
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean