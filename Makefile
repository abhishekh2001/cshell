all: initialize

initialize: main.o io.o command_implementation.o parse_cmd.o misc.o sys_commands.o linkedlist.o history.o redirection.o env.o pipe.o
	gcc main.o io.o misc.o command_implementation.o parse_cmd.o sys_commands.o linkedlist.o history.o redirection.o env.o pipe.o -o shell.out

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

sys_commands.o: sys_commands.c
	gcc -c sys_commands.c

linkedlist.o: linkedlist.c
	gcc -c linkedlist.c

redirection.o: redirection.c
	gcc -c redirection.c

history.o: history.c
	gcc -c history.c

env.o: env.c
	gcc -c env.c

pip.o: pipe.c
	gcc -c pipe.c

clean:
	rm *.o
