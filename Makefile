CC=gcc -std=c2x

TESTS=arena bstring random string_builder
test: $(foreach test, $(TESTS), test/$(test))

tests/%: tests/%.c ctest.h
	$(CC) -o $@ $< 

test/%: tests/%
	./$<
