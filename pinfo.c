#include "pinfo.h"
#include "prompt.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void pinfo(char *prompt)
{
    struct command cmd = getArgv(prompt);

    int pid = getpid();
    char *string_pid;

    // Select the process id based on the arguments
    if (cmd.num == 1)
    {
        int len = snprintf(NULL, 0, "%d", pid);
        string_pid = (char *)calloc(len, sizeof(char));
        sprintf(string_pid, "%d", pid);
    }
    else
    {
        int len = strlen(cmd.str_vec[1]);
        string_pid = (char *)calloc(len, sizeof(char));
        strcpy(string_pid, cmd.str_vec[1]);
    }
    
    // Constructing path of the proc stat file: /proc/{pid}/stat
    char procFile[11 + strlen(string_pid)];

    strcpy(procFile, "/proc/");
    strcat(procFile, string_pid);
    strcat(procFile, "/stat");

    // Opening the proc file
    int proc_fd = open(procFile, O_RDONLY);

    if (proc_fd == -1)
    {
        printf("pinfo: process with pid = %s does not exist\n", string_pid);
        return;
    }

    // Reading the proc file into buffer
    int buf_size = 10240;
    char *buffer = (char *)calloc(buf_size, sizeof(char));

    read(proc_fd, buffer, 10240);
    struct command params = getArgv(buffer);

    // Printing the necessary details
    printf("pid : %s\n", params.str_vec[0]);
    printf("process Status : %s%s\n", params.str_vec[2], strcmp(params.str_vec[4], params.str_vec[7]) == 0 ? "+" : "");
    printf("memory : %s\n", params.str_vec[22]);

    // Constructing path of the proc exe file: /proc/{pid}/exe
    char exeFile[10 + strlen(string_pid)];

    strcpy(exeFile, "/proc/");
    strcat(exeFile, string_pid);
    strcat(exeFile, "/exe");

    // Get the executable path from exe file in proc folder
    char exe_path[PATH_MAX];
    int len = readlink(exeFile, exe_path, PATH_MAX);

    bool contain_home = true;

    // Modifying the exetuable path according to home path
    int len_home = strlen(home_path);
    for (int i = 0; i < len_home; ++i)
    {
        if (exe_path[i] != home_path[i])
        {
            contain_home = false;
            break;
        }
    }

    if (!contain_home)
    {
        printf("executable Path : %s\n", exe_path);
    }
    else
    {
        printf("executable Path : ~%s\n", exe_path + len_home);
    }
}