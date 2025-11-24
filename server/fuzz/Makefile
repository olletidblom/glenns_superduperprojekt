# Fuzzing Makefile
CC = clang
CFLAGS = -g -O1 -fsanitize=fuzzer,address -I../src -I../../libs
AFL_CC = afl-clang-fast
AFL_CFLAGS = -fsanitize=address,undefined -g -I../src -I../../libs
SRC_FILES = ../src/HTTPServerConnection.c ../src/HTTPServer.c ../src/TCPServer.c ../src/Handlers/weather_handler.c ../../libs/smw.c ../../libs/TCPClient.c
# AFL target
afl_http_parser: afl_http_parser.c
	$(AFL_CC) $(AFL_CFLAGS) afl_http_parser.c $(SRC_FILES) -o afl_http_parser

# AFL full flow target
afl_http_flow: afl_http_flow.c
	$(AFL_CC) $(AFL_CFLAGS) afl_http_flow.c $(SRC_FILES) -o afl_http_flow

# Run LibFuzzer
run: fuzz_http_parser
	mkdir -p corpus
	./fuzz_http_parser corpus/ -max_total_time=60

# Run AFL parser only
afl-run: afl_http_parser
	mkdir -p afl_input afl_output
	@if [ ! -f afl_input/test1.txt ]; then \
		echo -e "GET /test HTTP/1.1\r\nHost: example.com\r\nContent-Length: 0\r\n\r\n" > afl_input/test1.txt; \
		echo -e "POST /api HTTP/1.1\r\nHost: api.example.com\r\nContent-Length: 10\r\n\r\n" > afl_input/test2.txt; \
	fi
	AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 afl-fuzz -i afl_input -o afl_output ./afl_http_parser

# Run AFL full flow (ParseHeader + BuildURL + HandleRequest)
afl-run-flow: afl_http_flow
	mkdir -p afl_input afl_output_flow
	@if [ ! -f afl_input/test1.txt ]; then \
		echo -e "GET /test HTTP/1.1\r\nHost: example.com\r\nContent-Length: 0\r\n\r\n" > afl_input/test1.txt; \
		echo -e "POST /api HTTP/1.1\r\nHost: api.example.com\r\nContent-Length: 10\r\n\r\n" > afl_input/test2.txt; \
	fi
	AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 afl-fuzz -i afl_input -o afl_output_flow ./afl_http_flow

# Clean
clean:
	rm -f fuzz_http_parser afl_http_parser afl_http_flow
	rm -rf corpus/ crash-* leak-* timeout-* afl_input/ afl_output/ afl_output_flow/

.PHONY: run afl-run afl-run-flow clean
