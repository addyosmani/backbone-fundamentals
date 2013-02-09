TEST_DIR = test

all: test

test:
	@@echo "Running all tests via vows"
	@@vows ${TEST_DIR}/*-test.js

bench:
	@@echo "Running benchmark on big.css file"
	@@node test/bench.js

.PHONY: all test bench