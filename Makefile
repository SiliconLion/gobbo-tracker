CC = gcc
CFLAGS = -std=c11 -Wall -Iinclude 

gobbo-tracker.exe: src/main.c src/fighter.c
	$(CC) -o gobbo-tracker.exe src/main.c src/fighter.c `pkg-config --cflags gtk+-3.0`  `pkg-config --libs gtk+-3.0` 

# build:
# `pkg-config --cflags gtk+-3.0`  `pkg-config --libs gtk+-3.0` 