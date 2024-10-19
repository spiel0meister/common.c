CC=gcc -std=c11
CFLAGS=-Wall -Wextra -Wpedantic -ggdb -I./src/ -fsanitize=address,leak
LIBS=-lcurl

all: testc 

MODS=arena cperf da string_builder string_view leakcheck
testc: main.c $(foreach mod, $(MODS), src/$(mod).h)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

%/:
	mkdir -p $@

clean:
	rm -rf testc 

