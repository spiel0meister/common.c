CC=gcc -std=c2x

test: src/main.c
	$(CC) -ggdb -o $@ $^ 
