#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "pish.h"

static char pish_history_path[1024] = {'\0'};

/*
 * Set history file path to ~/.pish_history.
 */
static void set_history_path()
{
    const char *home = getpwuid(getuid())->pw_dir;
    strncpy(pish_history_path, home, 1024);
    strcat(pish_history_path, "/.pish_history");
}

void add_history(const struct pish_arg *arg)
{
    FILE *history_file;
    // set history path if needed
    if (!(*pish_history_path)) {
        set_history_path();
    }

    history_file = fopen(pish_history_path, "a");
    if(history_file == NULL) {
        history_file = fopen(pish_history_path, "w");
    }
    fprintf(history_file, "%s", arg->argv[0]);
    for(int j = 1; j < arg->argc; j++) {
        fprintf(history_file, " %s", arg->argv[j]);
    }
    fprintf(history_file, "\n");
    fclose(history_file);
}

void print_history()
{
    // set history path if needed
    if (!(*pish_history_path)) {
        set_history_path();
    }

    FILE *print_history = fopen(pish_history_path, "r");
    if(print_history == NULL) printf("History did not open");
    char buf2[1024];
    int index = 1;
    while(fgets(buf2, sizeof(buf2), print_history)) {
        printf("%d %s", index , buf2);
        index++;
    }


    fclose(print_history);
}
