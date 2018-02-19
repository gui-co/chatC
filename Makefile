CC = gcc
CFLAGS = -Wall
LDLIBS = -lcurses
EXEC = serveur client

all: $(EXEC)

serveur: serveur.o sock.o
	$(CC) -o $@ $^ $(LDLIBS)

client: client.o sock.o
	$(CC) -o $@ $^ $(LDLIBS)

clean:
	rm *.o
