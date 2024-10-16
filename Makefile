CC=gcc -std=c11
CFLAGS=-Wall -Wextra -ggdb -I./src/ -fsanitize=address,leak
LIBS=-lcurl

CPP=g++ -std=c++17
CPPFLAGS=-Wall -Wextra -ggdb -I./src/

all: testc testcpp

MODS=arena cperf da string_builder string_view leakcheck
testc: main.c $(foreach mod, $(MODS), src/$(mod).h)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

testcpp: main.cpp $(foreach mod, $(MODS), src/$(mod).h)
	$(CPP) $(CPPFLAGS) -o $@ $<

%/:
	mkdir -p $@

clean:
	rm -rf testc testcpp

