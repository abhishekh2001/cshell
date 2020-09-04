all: initialize

initialize: main.o io.o command_implementation.o parse_cmd.o misc.o
	gcc main.o io.o misc.o command_implementation.o parse_cmd.o -o shell.out

main.o: main.c
	gcc -c main.c

io.o: io.c
	gcc -c io.c

command_implementation.o: command_implementation.c
	gcc -c command_implementation.c

parse_cmd.o: parse_cmd.c
	gcc -c parse_cmd.c

misc.o: misc.c
	gcc -c misc.c