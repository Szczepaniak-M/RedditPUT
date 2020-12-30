CC = gcc
CFLAGS= -Wall -l sqlite3

all: server

server: server.c structures.h database.c database.h
	$(CC) $(CFLAGS) server.c database.c -o server

