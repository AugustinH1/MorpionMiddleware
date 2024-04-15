CC = gcc
CFLAGS = -Wall -Wextra

all: client.exe serveur.exe

morpion.o: lib/morpion.c
	$(CC) $(CFLAGS) -c lib/morpion.c -o lib/morpion.o

client.exe: client.c lib/morpion.o
	$(CC) $(CFLAGS) client.c lib/morpion.o -o client.exe -l INET -L libINET

serveur.exe: serveur.c lib/morpion.o
	$(CC) $(CFLAGS) serveur.c lib/morpion.o -o serveur.exe -l INET -L libINET

clean:
	rm -f *.exe lib/*.o