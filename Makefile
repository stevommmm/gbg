BUILD := standard

CC = gcc
cflags.common := -g -Wall -Wuninitialized -O1 -std=c11 -lX11 -lImlib2
cflags.debug := -fsanitize=address -fno-omit-frame-pointer
cflags.compositing := -DNO_FINAL_FREE
CFLAGS := ${cflags.${BUILD}} ${cflags.common}
BINARY = gbg

all: $(BINARY)

$(BINARY): $(wildcard *.c)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	$(RM) */*.o *.o $(BINARY)
