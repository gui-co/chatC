CC = gcc
CFLAGS = -Wall
LDLIBS = -lcurses
EXEC = serveur client

all: $(EXEC)

serveur: serveur.o sock.o erreur.o
	$(CC) -o $@ $^ $(LDLIBS)

client: client.o sock.o erreur.o
	$(CC) -o $@ $^ $(LDLIBS)

clean:
	rm *.o
