CC=gcc -std=c11

test: src/main.c
	$(CC) -ggdb $^ -o $@
