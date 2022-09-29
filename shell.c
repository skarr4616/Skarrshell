#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <ctype.h>
#include <sys/wait.h>
#include <time.h>

#include "prompt.h"
#include "history.h"

char userName[256] = {'\0'};

char nline[2] = "\\n";
char mod_path[PATH_MAX] = "~";
int path_has_home = true;
struct processInfo *pHead = NULL;

int nextPos = 0;
int startPos = 0;

int bg_count = 0;
bool fg_running = false;
bool fg_killed = false;

int main()
{
    // Get the username and system
    getlogin_r(userName, 256);
    gethostname(sysName, HOST_NAME_MAX + 1);

    // Get the home directory
    getcwd(home_path, PATH_MAX);
    getcwd(curr_path, PATH_MAX);
    getcwd(prev_path, PATH_MAX);

    // Get the history of the previous sessions
    openHistoryFile();

    // Print the prompt for the first time
    printNames();

    // Handle finished background processes: Print process name along with pid and exit status
    signal(SIGCHLD, SIG_IGN);
    signal(SIGCHLD, end_bg_process);

    signal(SIGINT, SIG_IGN);
    signal(SIGINT, end_fg_process);

    signal(SIGTSTP, SIG_IGN);
    signal(SIGTSTP, stop_fg_process);

    fg_Proc.job_Num = 0;
    fg_Proc.pid = -1;

    while (true)
    {
        getcwd(curr_path, PATH_MAX);

        // Take input
        char prompt[MAX_ARG_LEN] = {'\0'};
        strcpy(prompt, getPrompt());

        // Store the input in history and update the history file
        if (strlen(prompt) > 0)
            updateHistory(prompt);

        // Parse the string
        parseString(prompt);

        printNames();
    }

    return 0;
}