# Skarrshell

## _To run the shell_

```
$ make
$ ./a.out
```

# Structure of the Code

The code is comprised of command.h and command.c files.

Every ".c" file involved in making the shell has been explained below:

- makefile 
    - This file makes it easy for the user to recompile the code

- shell.c 
  
    - Starting point of the shell.
    - Gets the username, hostname, path to current working directory and previous session history.
    - The file is responsible for getting the user input at the prompt.
    - The input is given to ```updateHistory()``` function and ```parseString()``` function, and the loop continues.

- prompt.c

    - Contains functions to get print prompt, user input, parse user input, execute function to start background and foreground processes and signal handlers.

- cd.c, echo.c, history.c, discover.c, ls.c, pinfo.c

    - Implement the commands given in the assignment manual.

# All implemented functions

| Name | Functionality |
| ------ | ------ |
| getArgv() | Returns a struct after tokenizing the string argument with space and tab |
| printNames() | Print the prompt on the shell |
| getPrompt() | Gets the user input |
| parseString() | Tokenize the string with ';' as the delimiter |
| execCommand() | Tokenizes the string with '&' and creates child processes for background commands. Background process commands are passed to ```execvp()``` after forking|
| execute() | If the command is an implemented function, calls the written function to run it. Else creates a child process and waits for it to complete execution. After forking, command is passed to ```execvp()```. |
| updatePlist() | Adds the new background process to the list of already running processes. |
| deleteProcess() | Deletes a process with a given pid from the list of processes |
| getProcess() | Returns the name and start time of the process with the given pid |
| end_bg_process() | Signal handler for background process termination |
| cd() | Implements ```cd``` command |
| discover() | Primary function to get the required arguments before implementing the function |
| printDiscoverAll() | Recursively prints corresponding files and directories if name is not given |
| printDiscoverName() | Recursively prints corresponding files and directories if name is given |
| echo() | Implements ```echo`` command |
| openHistoryFile() | Function to get history of previous sessions from history.txt file |
| updateHistory() | Function to update the history array and file when new commands are entered |
| UpdateHistoryFile() | Update the history file before exiting the shell |
| printHistory() | Prints the last 10 commands |
| pinfo() | Implement the ```pinfo``` command |

# Code Flow

- Any input given is first taken by main.c, it is added to the history
- The input is then given to the parseString() which divides the input into commands separated by semicolons.
- Then the command is further handed over to execCommand() which tokenizes further based on '&' and then executes foreground and background processes accordingly.
- Then the command is further tokenized based on the spaces, tabs etc and then is decided upon if it is built-in or system command
- If it is built-in, the required functionality is called
- If it is foreground, the child executes the process using execvp, parent waits
- If it is background, the child executes the process using execvp, parent doesn't wait and continues the implementation.
- The next prompt is printed for the user to give input and the loop continues

# Assumptions

1. Path of any directory is less atmost 256 characters long
2. User input is atmost of 256 characters long
3. The username and hostname are atmost 256 characters long
4. The input arguments to every function haven't been mentioned in the README.md file, please go through the code to see the implementation
5. The discover command doesn't show hidden files
6. Time of execution is shown for both background and foreground process only one time in a newline after execution is completed/terminated
7. If only space character is given as input command, it is also stored in history as a valid command
8. ```echo``` command imitates the actual command by retaining extra spaces inside inverted commas, removing the ones outside it and not printing the inverted commas.

