CC = gcc
CFLAGS = -g -Wall -Wuninitialized -O1 -std=c11 -lX11 -lImlib2
ifdef SANATIZE_ADDRESS
	CFLAGS += -fsanitize=address -fno-omit-frame-pointer
endif
BINARY = gbg

all: $(BINARY)

$(BINARY): $(wildcard *.c)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	$(RM) */*.o *.o $(BINARY)
