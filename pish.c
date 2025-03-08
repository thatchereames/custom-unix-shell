#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "pish.h"

#define CHAR_SEPERATORS " \n \t "

/*
 * Batch mode flag. If set to 0, the shell reads from stdin. If set to 1,
 * the shell reads from a file from argv[1].
 */
static int script_mode = 0;

/*
 * Prints a prompt IF NOT in batch mode (see script_mode global flag),
 */ 
int prompt(void)
{
    static const char prompt[] = {0xe2, 0x96, 0xb6, ' ', ' ', '\0'};
    if (!script_mode) {
        printf("%s", prompt);
        fflush(stdout);
    }
    return 1;
}

/*
 * Print usage error for built-in commands.
 */
void usage_error(void)
{
    fprintf(stderr, "pish: Usage error\n");
    fflush(stdout);
}

/*
 * Break down a line of input by whitespace, and put the results into
 * a struct pish_arg to be used by other functions.
 *
 * @param command   A char buffer containing the input command
 * @param arg       Broken down args will be stored here.
 */
void parse_command(char *command, struct pish_arg *arg)
{
    char *token = strtok(command, CHAR_SEPERATORS);
    int i;
    for(i = 0; token != NULL; i++) {
        arg->argv[i] = malloc((strlen(token) + 1) * sizeof(char));
        strcpy(arg->argv[i], token);
        token = strtok(NULL, CHAR_SEPERATORS);
    }
    arg->argv[i] = NULL;
    arg->argc = i;
}

/*
 * Run a command.
 *
 * Built-in commands are handled internally by the pish program.
 * Otherwise, use fork/exec to create child process to run the program.
 *
 * If the command is empty, do nothing.
 * If NOT in batch mode, add the command to history file.
 */
void run(struct pish_arg *arg)
{
    int pid = fork();
    if(pid == 0) {
        execvp(arg->argv[0], arg->argv);
        perror("pish");
    } else if(pid  > 0) {
        wait(NULL);
    } else {
        perror("fork");
    }
}

/*
 * The main loop. Continuously reads input from a FILE pointer
 * (can be stdin or an actual file) until `exit` or EOF.
 */
int pish(FILE *fp)
{
    // assume input does not exceed buffer size
    char buf[1024];
    struct pish_arg *arg = malloc(sizeof(struct pish_arg));

    while (prompt() && fgets(buf, sizeof(buf), fp)) {
        //Not sure if this is the correct implementation but it is along this idea 🫡
        parse_command(buf, arg);
        if(arg->argc == 0) ;
        else if(!strcmp(arg->argv[0], "exit")) break;
        else if(!strcmp(arg->argv[0], "cd")) {
            if(chdir(arg->argv[1]) != 0) perror("cd");
        }
        else if(!strcmp(arg->argv[0], "history")) printf("History will be implemented later :(\n");
        else run(arg);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    
    if(argc == 1) {
        fp = stdin;
        script_mode = 0;
    }
    else if(argc == 2) {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            perror("open");
            return EXIT_FAILURE;
        }
        script_mode = 1;
    } else usage_error();
    
    pish(fp);

    if(script_mode) fclose(fp);
    return EXIT_SUCCESS;
}
