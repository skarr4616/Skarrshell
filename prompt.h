/**
* @file prompt.h
* @author Souvik Karfa (2020102051)
* @brief 
* @version 0.1
* @date 27-08-2022
* @copyright Copyright (c) 2022
*/

#ifndef __PROMPT_H
#define __PROMPT_H

#include <sys/stat.h>
#include <limits.h>

typedef int bool;

#define true 1
#define false 0

#define MAX_ARG_LEN 256

#define BLUE          "\x1B[34m"
#define RED           "\x1B[31m"
#define GREEN         "\x1B[32m"
#define RESET         "\x1B[0m"

struct command {
    int num;
    char *str_vec[MAX_ARG_LEN];
};

struct processInfo {
    int job_Num;
    int pid;
    char pName[MAX_ARG_LEN];
    int seconds;
    struct processInfo* next;
};

char userName[256];
char sysName[HOST_NAME_MAX + 1];

char home_path[PATH_MAX];
char curr_path[PATH_MAX];
char prev_path[PATH_MAX];
char mod_path[PATH_MAX];

int path_has_home;

int numProcess;
int bg_count;

bool fg_running;
bool fg_killed;
bool ctrl_c;

struct processInfo* pHead;
struct processInfo* pNext;

struct processInfo fg_Proc;

int stdin_save;
int stdout_save;

struct command getArgv(char *prompt);
void printNames();
char* getPrompt();
void parseString(char* prompt);
void execCommand(char* prompt);
void execute(char* prompt);
void delProcess(int pid);
void updatePlist(int pid, char* prompt);
struct processInfo *getProcess(int pid);
struct processInfo *getProcessJob(int job);
void pKill(char* prompt);
void jobs(char* prompt);
void fg(char *prompt);
void end_bg_process(int sig);
void end_fg_process(int sig);
void stop_fg_process(int sig);

#endif
