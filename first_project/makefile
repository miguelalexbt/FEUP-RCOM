CC=gcc 
CFLAGS=-Wall -Wextra -Werror -lm -O2

default:
	$(CC) main.c -Iinclude src/*/*.c -o serial $(CFLAGS)

clean:
	-rm serial