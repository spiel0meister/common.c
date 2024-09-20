CC=gcc -std=c11
CFLAGS=-Wall -Wextra -ggdb -I./src/
LIBS=-lcurl

CPP=g++ -std=c++17
CPPFLAGS=-Wall -Wextra -ggdb -I./src/

all: testc testcpp

MODS=arena cperf da log string_builder string_view leakcheck
testc: main.c $(foreach mod, $(MODS), src/$(mod).h)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

testcpp: main.cpp $(foreach mod, $(MODS), src/$(mod).h)
	$(CPP) $(CPPFLAGS) -o $@ $<

%/:
	mkdir -p $@
