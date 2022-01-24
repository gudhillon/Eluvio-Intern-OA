CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wpedantic

all: strand

strand: strand.o
	$(CC) -o strand strand.o

strand.o: strand.c
	$(CC) $(CFLAGS) -c strand.c

clean:
	rm -rf strand strand.o infer-out

