CC = gcc
CFLAGS = -O0 -Iinclude/ -std=c11 -Wall -Wextra -Wpedantic
SRCFILES = src/main.c src/fighter.c src/utilities/lowlevel.c src/utilities/vector.c

gobbo-tracker.exe: $(SRCFILES)
	$(CC) -g3 -o gobbo-tracker.exe $(SRCFILES) `pkg-config --cflags gtk+-3.0`  `pkg-config --libs gtk+-3.0` 

