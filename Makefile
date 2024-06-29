CC=gcc
CFLAGS=-Wall -Wextra -ggdb

MODS=arena cperf da log string_builder string_view
test: main.c $(foreach mod, $(MODS), src/$(mod).h)
	gcc $(CFLAGS) -o $@ $<

TESTS=arena string_builder
tests: $(foreach test, $(TESTS), tests/$(test) test/$(test))

tests/%: tests/%.c ctest.h
	$(CC) $(CFLAGS) -o $@ $< 

test/%: tests/%
	./$<
