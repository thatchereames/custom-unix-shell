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
#define HISTORY_FILE ".pish_history"

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
    char buf[1024];
    struct pish_arg *arg = malloc(sizeof(struct pish_arg));
    FILE *history;
    //Prepares to add to history if pish called in command mode
    if(!script_mode) history = fopen(HISTORY_FILE, "a");

    while (prompt() && fgets(buf, sizeof(buf), fp)) {
        parse_command(buf, arg);
        if(arg->argc == 0) ;
        else if(!strcmp(arg->argv[0], "exit")) {
            if(arg->argc != 1) usage_error();
            else {
                if(!script_mode) {
                    fprintf(history, "%s\n", arg->argv[0]);
                }
                free(arg->argv[0]);
                break;
            }
        }
        else if(!strcmp(arg->argv[0], "cd")) {
            if(arg->argc != 2) usage_error();
            else if(chdir(arg->argv[1]) != 0) perror("cd");
            else if (!script_mode) fprintf(history, "%s %s\n", arg->argv[0], arg->argv[1]);
        }
        else if(!strcmp(arg->argv[0], "history")) {
            if (arg->argc != 1) usage_error();
            else {
                fprintf(history, "%s\n", arg->argv[0]);
                fflush(history);
                FILE *print_history = fopen(HISTORY_FILE, "r");
                if(print_history == NULL) printf("History did not open");
                char buf2[1024];
                int index = 1;
                while(fgets(buf2, sizeof(buf2), print_history)) {
                    printf("%d %s", index , buf2);\
                    index++;
                }
            }
        }
        else {
            run(arg);
            fprintf(history, "%s", arg->argv[0]);
            for(int j = 1; j < arg->argc; j++) {
                fprintf(history, " %s", arg->argv[j]);
            }
            fprintf(history, "\n");
        }

        for(int j = 0; arg->argv[j] != NULL; j++) {
            free(arg->argv[j]);
            arg->argv[j] = NULL;
        }
    }
    if(!script_mode) fclose(history);
    free(arg);
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
