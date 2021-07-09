# Shell assignment

## Overview
Creatively named, the aim of "Shell assignment" shell is to implement a simplified version of the shell for the course "Operating Systems and Networks".

## Compilation instructions
Invoke `make` from the directory containing the source code (here, `shell-assignment`) which creates an executable `shell.out` in the same directory

`make clean` can be performed to remove `*.o` files.

The shell itself on execution requires `.history.txt` file to be created (or accessed if one already exists) from the directory it is executed 
to allow for persistent history across shell instances.

Has been tested on `gcc version 9.2.1 20191008 (Ubuntu 9.2.1-9ubuntu2)`

## Specifications

The "shell assignment" shell provides a basic set of features as listed by the specification requirements.

### Assignment 2

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

### Assignment 3

1. Input/Output Redirection: Using the symbols <, > and >>, the output of commands, usually written to
stdout, can be redirected to another file, or the input taken from a file other
than stdin.

2. Command Pipelines: A pipe, identified by |, redirects the output of the command on the left as
input to the command on the right.

3. I/O Redirection within Command Pipelines: Input/output redirection can occur within command pipelines.

4. User-defined Commands:
    - `setenv var [value]`: This command allows the user to set the value of the
environment variable `var` to value, creating the environment variable if
it doesn't already exist

    - `unsetenv var`: Destroys the env variable `var`, if it exists.

    - `jobs`: Prints a list of all currently running background. The job number assigned by the shell is incremented and
      a number that has been assigned once can not be assigned once again.

    - `kjob <job number> <signal number>`: Takes the job number (assigned by your shell) of a running job and
sends the signal corresponding to signal number to that process

    - `fg <job number>`: Brings the running or stopped background job corresponding to job
number to the foreground, and changes its state to running.

    - `bg <job number>`: Changes the state of a stopped background job to running (in the
background).

    - `overkill`: Kills all background processes. Called on performing a `quit`.

    - `quit`: Exits the shell (equivalent to `ctrl-d`).

5. Signal Handling

- `CTRL-Z`: Pushes any currently running foreground job into the
background, and change its state from running to stopped.

- `CTRL-C`: Interrupt any currently running foreground job.

6. Last Working Directory: `cd -` takes the user to the last working directory.

7. Exit Codes: A smiley or a sad face at the start of the prompt indicates whether
the last command executed exitted successfuly or not, respectively.


## Assumptions

The following assumptions have been made regarding the specification requirements and the
behaviour expected.

- When executing commands separated by semicolons, expected behaviour is to execute commands in 
order i.e, wait for the first command to finish execution before executing the second and so on...,
unless of course, the first were to be a background process in which case termination of the 
background process is not required before moving on to the next command.

- Arguments to system commands will not contain `~`, since an inplace replacement of 
~ with the `homedir` requires a simple call to the already implemented `handle_tilda`
function found in `misc.c`. For example, `vi ~/text.txt` will not be tested upon.

- History syncronization between *concurrently* executing shells (including nested) is not
required.

- The `/proc/<pid>/*` files do not vary accross distrubtions (some keywords are hardcoded
like `State` and `VmSize`).

- An empty command is considered successful.

- A process' job number, once assigned, cannot be changed unless it
is taken out and put back into the background process queue (`fg` command).
This behaviour is assumed since the assignment pdf does not 
elaborate on subsequent assignment of job numbers to future programs
when existing programs have terminated.

- A job number, that has ever been assigned to a process in the 
lifetime of the shell execution, can never be reassigned to any
other process.

- On quitting, all background processes are to be killed

- The `diff` command results in a failure if the files are not
the same.

- Empty commands need not be stored as history.

- Invalid commands (non-existent programs) that are attempted to
run in the background are immediately killed.

- `ctrl-c` on a running foreground process is not a failed exit. Behaviour of 
smiley/sad face on `ctrl-c` and `ctrl-z` on shell prompt is not 
defined.

Note: The linked list is a modification of 
[https://github.com/skorks/c-linked-list](https://github.com/skorks/c-linked-list/)
since implementing one (although relatively simple) is timeconsuming, prone to errors and is not the main objective
of this assignment. 

## Files

- `main.c`: Initialization of global variables and data structures, main loop, and some cleanup on quitting

- `command_implementation.c`: Implementation of all built-in commands (from assignment two and three, and a wrapper for calling `execvp`). 
In hindset, this file should have been split but considering that it
still only takes up a reasonable number of lines, it will suffice for this assignment (at the least).

- `history.c`: Deals with reading/writing to `.history.txt` and storing commands in the history.

- `io.c`: Functions that relate to displaying the prompt and tokenizing input resides in this file.

- `linkedlist.c`: Interface for dealing with lined lists needed for storing background processes

- `misc.c`: Just random things like dealing with the tilda character and maybe some utility helper functions will
reside here in the future when required.

- `parse_cmd.c`: Deals with tokenizations and calling necessary functions depending on the command entered.

- `sys_commands.c`: Implements foreground, background processes and the `jobs` command, along with a signal handler in the case
of termination of a child process. Implements a handler for `ctrl-c` and `ctrl-z`. 

- `env.c`: Responsible for functions that set and get the user defined environment variables

- `redirection.c`: Handles all functionalities pertaining to redirection

- `pipe.c`: Implements capabilities that allow piping of commands.

