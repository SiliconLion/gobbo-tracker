CC = gcc
CFLAGS = -std=c11 -Wall -Iinclude 

gobbo-tracker: src/main.c
	$(CC) -o gobbo-tracker src/main.c `pkg-config --cflags gtk+-3.0`  `pkg-config --libs gtk+-3.0` 

# build:
# `pkg-config --cflags gtk+-3.0`  `pkg-config --libs gtk+-3.0` 