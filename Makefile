CC=gcc -std=c2x

test: main.c
	gcc -Wall -Wextra -ggdb -o $@ $^

TESTS=arena bstring random string_builder
tests: $(foreach test, $(TESTS), test/$(test))

tests/%: tests/%.c ctest.h
	$(CC) -o $@ $< 

test/%: tests/%
	./$<
