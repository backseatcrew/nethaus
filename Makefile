CC=gcc
CFLAGS=-Wall
DEPS = sendrecieve.h
OBJ= client.o server.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

make: $(OBJ)
	$(CC) -o make client.o
	$(CC) -o make server.o

.PHONY : clean
clean:	
	-rm *.o
