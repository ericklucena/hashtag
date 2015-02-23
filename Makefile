#Author: Erick Lucena Palmeira Silva
CC=gcc
CPPFLAGS=
CFLAGS= -std=c99 -Wall -pedantic -Iinclude -g
LDFLAGS=

all: src/*.c
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o bin/hashtag

clean: 
	rm bin/hashtag
