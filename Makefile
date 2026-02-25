CC := gcc
CFLAGS := -Wall -Wextra -std=c99

TESTS := 01-parse-char
TEST_BINS := $(TESTS:%=tests/%.bin)

.PHONY: test clean

test: $(TEST_BINS)
	@fail=0; \
	for t in $(TEST_BINS); do \
		echo "Running $$t"; \
		./$$t || fail=1; \
	done; \
	exit $$fail

tests/%.bin: tests/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f tests/*.bin