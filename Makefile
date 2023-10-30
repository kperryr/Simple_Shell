# choose your compiler
CC=gcc
#CC=gcc -Wall

mysh: sh.o get_path.o main.o 
	$(CC) -g main.c sh.o get_path.o -o mysh
#	$(CC) -g main.c sh.o get_path.o bash_getcwd.o -o mysh

sh.o: sh.c sh.h
	$(CC) -g -c sh.c

get_path.o: get_path.c get_path.h
	$(CC) -g -c get_path.c

clean:
	rm -rf sh.o get_path.o mysh
