CC=gcc
CFLAGS=-g -Wall
DEPS = sendrecieve.h
OBJ= client.o server.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

make: $(OBJ)
	$(CC) -o make client.o -o client
	$(CC) -o make server.o -o server

.PHONY : clean
clean:	
	-rm *.o
