CC=gcc
FLAGS=-g -Wall

interp:interp.c
	$(CC) $(FLAGS) -o interp interp.c
