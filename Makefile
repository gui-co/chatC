CFLAGS = -Wall
LIBS = -lcurses

all: serveur client

serveur: serveur.o sock.o
	gcc -o $@ $^ $(LIBS)

client: client.o sock.o
	gcc -o $@ $^ $(LIBS)

clean:
	rm *.o
