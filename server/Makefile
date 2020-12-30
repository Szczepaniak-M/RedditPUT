CC = gcc
CFLAGS= -Wall -l sqlite3


all: main.c structures.h server.c server.h database.c database.h thread.c thread.h
	$(CC) $(CFLAGS) main.c server.c database.c thread.c -o server

