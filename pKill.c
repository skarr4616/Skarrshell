#include "pKill.h"
#include "prompt.h"

#include <stdio.h>
#include <signal.h>

// Signals the process with the given job id
void pKill(char *prompt)
{
    struct command cmd = getArgv(prompt);

    int job;
    int sig;

    sscanf(cmd.str_vec[1], "%d", &job);
    sscanf(cmd.str_vec[2], "%d", &sig);

    struct processInfo *p = getProcessJob(job);

    if (p == NULL)
    {
        printf("sig: process with job no %d does not exist\n", job);
        return;
    }

    kill(p->pid, sig);
}
