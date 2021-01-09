# CShell

## Overview
Creatively named, the aim of "cshell" shell is to implement a simplified version of the shell.

## Compilation instructions
Invoke `make` from the directory containing the source code which creates an executable `shell.out` in the same directory

`make clean` can be performed to remove `*.o` files.

The shell itself on execution requires `.history.txt` file to be created (or accessed if one already exists) from the directory it is executed 
to allow for persistent history across shell instances.

Has been tested on `gcc version 9.2.1 20191008 (Ubuntu 9.2.1-9ubuntu2)`

## Specifications

The "cshell" shell provides a basic set of features.

1. Display requirement: Prompt is of the format `<username@hostname: cwd>` as clarified in one of the forum inquiries.

2. Builtin commands: Supports `cd`, `pwd`, `echo`. `cd` or `cd ~` takes the shell to the pseudohome directory.

3. ls command: The built-in `ls` command takes the format `ls -[al] [directory(ies)]`. It does not support any of the other
options that fully blown shells provide. Moreover, it does not sort the outputs in lexicographic order.

4. System commands with and without arguments: Allows system commands (non built-in commands) to run in either foreground or
background mode. 

5. pinfo command (user defined): Another built-in command, `pinfo [pid]` prints process related information of the process whose pid is the one
specified or the shell itself if no pid is provided.

6. Finished Background Processes: When a background process is done executing, the shell is interrupted and the details of the termination
(the process name, pid, and the termination status [normal/abnormal]) is displayed to the user.

7. History: `history [<num>]` command lists out the latest commands executed by the shell and all previous instances of the shell.
It remembers up to 20 of the latest commands called (irrespective of successful/unsuccessful execution).

## Files

- `main.c`: Initialization of global variables and data structures, main loop, and some cleanup on quitting

- `command_implementation.c`: Implementation of all built-in commands (`ls`, `cd`, `pwd`, `pinfo`, `echo`, `history`, and a wrapper for calling `execvp`). 
In hindset, this file should have been split but considering that it
still only takes up a reasonable number of lines, it will suffice for this assignment (at the least).

- `history.c`: Deals with reading/writing to `.history.txt` and storing commands in the history.

- `io.c`: Functions that relate to displaying the prompt and tokenizing input resides in this file.

- `linkedlist.c`: Interface for dealing with lined lists needed for storing background processes

- `misc.c`: Just random things like dealing with the tilda character and maybe some utility helper functions will
reside here in the future when required.

- `parse_cmd.c`: Deals with tokenizations and calling necessary functions depending on the command entered.

- `sys_commands.c`: Implements foreground and background processes along with a signal handler in the case
of termination of a child process.
