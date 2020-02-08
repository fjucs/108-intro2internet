CC=g++
CFLAGS=-std=c++14
LIBS=

all: server

%.o: %.cpp
	$(CC) -c $(CFLAGS) $(LIBS) $^ -o $@

server: main.o server.o utils.o
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

.PHONY: clean
clean:
	@rm -f ./server ./main ./*.o
	@echo Clean