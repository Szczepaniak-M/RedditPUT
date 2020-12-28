CC = gcc
CFLAGS= -Wall -l sqlite3

all: server

server: server.c structures.h
	$(CC) $(CFLAGS) server.c -o server
