CC = gcc
CFLAGS = -Wall -Wextra

all: client.exe serveur.exe

morpion.o: morpion.c
	$(CC) $(CFLAGS) -c morpion.c

client.exe: client.c morpion.o
	$(CC) $(CFLAGS) client.c morpion.o -o client.exe -l INET -L libINET

serveur.exe: serveur.c morpion.o
	$(CC) $(CFLAGS) serveur.c morpion.o -o serveur.exe -l INET -L libINET

clean:
	rm -f *.exe *.o