# HTTP Parser Fuzzing

This directory contains fuzzing harnesses for the HTTP server parser.

## Requirements

- Clang compiler with libFuzzer support
- AddressSanitizer (ASAN)

## Building

```bash
make
```

## Running

```bash
make run
```

This will:
- Create a corpus directory for test cases
- Run the fuzzer for 60 seconds
- Report any crashes, leaks, or timeouts

## Results

- `crash-*` - Inputs that caused crashes
- `leak-*` - Inputs that caused memory leaks
- `timeout-*` - Inputs that caused timeouts
- `corpus/` - Interesting inputs discovered by the fuzzer

## Analyzing Crashes

If a crash is found:
```bash
./fuzz_http_parser crash-<hash>
```
