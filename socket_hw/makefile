CC=g++
CFLAGS=-std=c++14
LIBS=

all: client

client: client.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

.PHONY: clean
clean:
	@rm -f ./client
	echo Clean