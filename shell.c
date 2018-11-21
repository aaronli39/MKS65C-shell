#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

char **parse_args(char *line) {
    char **args = calloc(sizeof(char *), 6);
    int index = 0;
    while (line != NULL && index < 5) {
        args[index] = strsep(&line, " ");
        index++;
    } return args;
}

void parse(char *line, char **argv) {
    while (*line != '\0') {       /* if not the end of line ....... */
        while (*line == ' ' || *line == '\t' || *line == '\n')
        *line++ = '\0';     /* replace white spaces with 0    */
        *argv++ = line;          /* save the argument position     */
        while (*line != '\0' && *line != ' ' &&
        *line != '\t' && *line != '\n')
        line++;             /* skip the argument until ...    */
    }
    *argv = '\0';                 /* mark the end of argument list  */
}

void execute(char **argv) {
    int pid;
    int status;

    if ((pid = fork()) < 0) {     /* fork a child process           */
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    }
    else if (pid == 0) {          /* for the child process:         */
        if (execvp(*argv, argv) < 0) {     /* execute the command  */
            printf("%s\n", strerror(errno));
            exit(1);
        }
    } else {                                  /* for the parent:      */
        while (wait(&status) != pid);       /* wait for completion  */
    }
}

void main(void) {
    char line[1024];             /* the input line                 */
    char *argv[64];              /* the command line argument      */

    while (1) {                   /* repeat until done ....         */
        printf("Shell -> ");     /*   display a prompt             */
        gets(line);              /*   read in the command line     */
        printf("\n");
        parse(line, argv);       /*   parse the line               */
        if (strcmp(argv[0], "exit") == 0)  /* is it an "exit"?     */
        exit(0);            /*   exit if it is                */
        execute(argv);           /* otherwise, execute the command */
    }
}
