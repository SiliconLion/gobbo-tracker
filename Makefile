CC = gcc
CFLAGS = -O0 -Iinclude/ -std=c11 -Wall -Wextra -Wpedantic

gobbo-tracker.exe: src/main.c src/fighter.c
	$(CC) -g3 -o gobbo-tracker.exe src/main.c src/fighter.c `pkg-config --cflags gtk+-3.0`  `pkg-config --libs gtk+-3.0` 

