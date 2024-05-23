CC=gcc -std=gnu1x

test: src/main.c
	$(CC) -ggdb -o $@ $^ 
