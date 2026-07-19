CC = gcc
CFLAGS = -Wall -Wextra -std=gnu17 -g

viksh: viksh.c utils.c builtins.c viksh.h
	$(CC) $(CFLAGS) viksh.c utils.c builtins.c -o viksh

run: viksh
	./viksh

clean:
	rm -f viksh

.PHONY: run clean