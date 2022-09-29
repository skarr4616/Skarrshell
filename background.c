#include "background.h"
#include "redirect.h"
#include "prompt.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void background(char *prompt)
{
    check_redirection(prompt);
    
    if (!do_redirection())
        return;
    
    struct command argv = getArgv(arg);

    int x = execvp(argv.str_vec[0], argv.str_vec);

    if (x == -1)
    {
        printf("%s: Command not found\n", argv.str_vec[0]);
    }

    exit(0);
}