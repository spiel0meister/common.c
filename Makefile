CC=gcc
CFLAGS=-Wall -Wextra -ggdb -I./src/

CPP=g++ -std=c++17
CPPFLAGS=-Wall -Wextra -ggdb -I./src/

all: testc testcpp

MODS=arena cperf da log string_builder string_view subprocess
testc: main.c $(foreach mod, $(MODS), src/$(mod).h)
	$(CC) $(CFLAGS) -o $@ $<

testcpp: main.cpp $(foreach mod, $(MODS), src/$(mod).h)
	$(CPP) $(CPPFLAGS) -o $@ $<

%/:
	mkdir -p $@
