#include "prompt.h"
#include "echo.h"
#include "cd.h"
#include "ls.h"
#include "history.h"
#include "discover.h"
#include "pinfo.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

// Function to tokenize the arguments with space and tab-space
struct command getArgv(char *prompt)
{
    char *token;
    char delim[] = {' ', '\t'};

    int i = 0;
    struct command cmd;
    token = strtok(prompt, delim);

    // Continuously tokenizing till we get a NULL
    while (token != 0)
    {
        cmd.str_vec[i] = token;
        token = strtok(0, delim);
        i++;
    }

    cmd.num = i;

    return cmd;
}

// Prints the username and system
void printNames()
{
    printf(GREEN "%s@%s" RESET ":" BLUE "%s" RESET "$ ", userName, sysName, mod_path);
}

// Takes the user input
char *getPrompt()
{
    fflush(stdout);
    char *prompt = (char *)calloc(MAX_ARG_LEN, sizeof(char));
    fgets(prompt, MAX_ARG_LEN, stdin);

    if ((strlen(prompt) > 0) && (prompt[strlen(prompt) - 1] == '\n'))
        prompt[strlen(prompt) - 1] = '\0';

    return prompt;
}

// Separates the commands on the basis of semicolon (;) and executes each of them seperately
void parseString(char *prompt)
{
    numProcess = 0;
    
    int len = strlen(prompt);
    prompt[len] = ';';
    prompt[len + 1] = '\0';

    char cmd[256][256] = {'\0'};
    
    int j = 0;
    int k = 0;

    for (int i = 0; i < strlen(prompt); ++i)
    {
        if (prompt[i] == ';')
        {
            k = 0;
            j++;
        }
        else
        {
            cmd[j][k] = prompt[i];
            k++;
        }
    }

    for (int i = 0; i < j; ++i)
    {
        execCommand(cmd[i]);
    }
}

// Separates the background and foreground commands on the basis of apersand (&) and run them appropriately
void execCommand(char *prompt)
{
    int len = strlen(prompt);
    prompt[len] = ' ';
    prompt[len + 1] = '\0';

    char *token;
    char delim[] = {'&'};
    token = strtok(prompt, delim);

    int j = 0;
    struct command backgrd;

    while (token != 0)
    {
        backgrd.str_vec[j] = (char *)malloc(strlen(token) * sizeof(char));
        backgrd.str_vec[j] = token;
        token = strtok(0, delim);
        j++;
    }

    backgrd.num = j;

    int forkRet;

    // Forking the background processes and executing them
    for (int i = 0; i < j - 1; ++i)
    {
        numProcess++;

        forkRet = fork();

        if (forkRet < 0)
        {
            printf("Error: Process fork failed\n");
            return;
        }
        else if (forkRet == 0)
        {
            setpgrp();
            struct command argv = getArgv(backgrd.str_vec[i]);

            int x = execvp(argv.str_vec[0], argv.str_vec);

            if (x == -1)
            {
                printf("%s: Command not found\n", argv.str_vec[0]);
            }

            exit(0);
        }
        else
        {
            // Printing the pid of newly created background process and storing its name
            printf("[%d] %d\n", numProcess, forkRet);
            updatePlist(forkRet, backgrd.str_vec[i]);
        }
    }

    // Executing foreground process
    execute(backgrd.str_vec[j - 1]);
}

// Execute the commands
void execute(char *prompt)
{
    // Findind the first argument of the command to identify its type
    int len = strlen(prompt);
    int i = 0;

    while (i < len && (prompt[i] == ' ' || prompt[i] == '\t'))
        i++;

    int l = 0;
    char cmd[MAX_ARG_LEN];
    while (prompt[i] != ' ' && prompt[i] != '\t' && prompt[i] != '\0')
    {
        cmd[l] = prompt[i];
        i++;
        l++;
    }

    cmd[l] = '\0';

    // Builtin commands
    if (strcmp(cmd, "cd") == 0)
    {
        cd(prompt);
        return;
    }

    if (strcmp(cmd, "pwd") == 0)
    {
        printf("%s\n", curr_path);
        return;
    }

    if (strcmp(cmd, "echo") == 0)
    {
        echo(prompt, i, len);
        return;
    }

    if (strcmp(cmd, "ls") == 0)
    {
        ls(prompt);
        return;
    }

    if (strcmp(cmd, "pinfo") == 0)
    {
        pinfo(prompt);
        return;
    }

    if (strcmp(cmd, "discover") == 0)
    {
        discover(prompt);
        return;
    }

    if (strcmp(cmd, "history") == 0)
    {
        printHistory();
        return;
    }

    if (strcmp(cmd, "exit") == 0)
    {
        exit(0);
    }

    if (cmd[0] == '\0')
        return;

    fg_running = true;

    // System commands
    time_t seconds = time(NULL);
    struct command argv = getArgv(prompt);

    int forkRet = fork();

    if (forkRet < 0)
    {
        printf("Error: Process fork failed\n");
        return;
    }
    else if (forkRet == 0)
    {
        int x = execvp(argv.str_vec[0], argv.str_vec);

        if (x == -1)
        {
            printf("%s: Command not found\n", argv.str_vec[0]);
            exit(1);
        }

        exit(0);
    }
    else
    {

        int status = 0;
        waitpid(forkRet, &status, WUNTRACED);

        fg_running = false;

        seconds = time(NULL) - seconds;

        if (seconds > 1)
        {
            printf("# %s for %ld seconds\n", argv.str_vec[0], seconds);
        }
    }

    return;
}

// Updates the process list when a new background process is created
void updatePlist(int pid, char *prompt)
{
    time_t dur = time(NULL);
    int len = strlen(prompt);
    int i = 0;

    while (i < len && (prompt[i] == ' ' || prompt[i] == '\t'))
        i++;

    int l = 0;
    char cmd[MAX_ARG_LEN];
    while (prompt[i] != ' ' && prompt[i] != '\t' && prompt[i] != '\0')
    {
        cmd[l] = prompt[i];
        i++;
        l++;
    }

    cmd[l] = '\0';

    if (pHead == NULL)
    {
        pHead = (struct processInfo *)calloc(1, sizeof(struct processInfo));

        pHead->pid = pid;
        strcpy(pHead->pName, cmd);
        pHead->seconds = dur;
        pHead->next = NULL;

        pNext = pHead;
    }
    else
    {
        pNext->next = (struct processInfo *)calloc(1, sizeof(struct processInfo));
        pNext = pNext->next;

        pNext->pid = pid;
        strcpy(pNext->pName, cmd);
        pNext->seconds = dur;
        pNext->next = NULL;
    }
}

// Deletes the process name of the given process id from our stored list
void delProcess(int pid)
{
    struct processInfo *temp = pHead;

    if (pHead->pid == pid)
    {
        pHead = pHead->next;
        free(temp);
        return;
    }

    while (temp->next != NULL)
    {
        if (temp->next->pid == pid)
        {
            struct processInfo *remove = temp->next;
            temp->next = temp->next->next;
            free(remove);
            return;
        }

        temp = temp->next;
    }
}

// Returns the process name of the given process id
struct processInfo *getProcess(int pid)
{
    struct processInfo *temp = pHead;

    while (temp != NULL)
    {
        if (temp->pid == pid)
        {
            return temp;
        }

        temp = temp->next;
    }
}

// Signal handler function to handle child death
void end_bg_process(int sig)
{
    int stat;
    int bg_pid = waitpid(-1, &stat, WNOHANG | WUNTRACED);

    if (bg_pid > 0)
    {
        struct processInfo *temp = getProcess(bg_pid);
        time_t dur = time(NULL) - temp->seconds;

        if (WIFEXITED(stat))
        {
            if (!fg_running && dur > 0)
                printf("\n");

            if ((WEXITSTATUS(stat)) == 0)
                printf("%s with pid = %d exited normally # After %ld seconds\n", temp->pName, bg_pid, dur);
            else
                printf("%s with pid = %d exited abnormally # After %ld seconds\n", temp->pName, bg_pid, dur);
        }
        else
        {
            if (!fg_running && dur > 0)
                printf("\n");

            printf("%s with pid = %d exited abnormally # After %ld seconds\n", temp->pName, bg_pid, dur);
        }

        if (!fg_running && dur > 0)
            printNames();

        delProcess(bg_pid);
        fflush(stdout);
    }
}
