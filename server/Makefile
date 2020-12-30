CC = gcc
CFLAGS= -Wall -pthread -l sqlite3


all: main.c structures.h server.c server.h database.c database.h
	$(CC) $(CFLAGS) main.c server.c database.c -o server

