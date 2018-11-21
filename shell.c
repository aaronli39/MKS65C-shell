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
    // if not end of line
    while (*line != '\0') {     
        // replace white spaces with 0    
        while (*line == ' ')
            *line++ = '\0';     
        // save the argument position
        *argv++ = line;          
        while (*line != '\0' && *line != ' ')
            // skip the argument until ... 
            line++;             
    }
    // end of argument list 
    *argv = '\0';                 
}

void execute(char **argv) {
    int status;
    int child = fork();

    if (child < 0) {     
        // fork child to do work ;-;
        printf("Error: %s\n", strerror(errno));
        exit(1);
    } else if (child == 0) {          
        // child: execute command
        if (execvp(*argv, argv) < 0) {   
            printf("%s\n", strerror(errno));
            exit(1);
        }
    } else {                                
        // parent: wait till complete
        wait(&status);     
    }
}

void main(void) {
    // store input
    char line[500];             
    // args
    char *argv[100];              

    while (1) {               
        // cmd line prompt    
        printf("> ");     
        gets(line);
        parse(line, argv);       
        // if exit exit, otherwise execute
        if (strcmp(argv[0], "exit") == 0)  
        exit(0);          
        execute(argv);        
        // printf("\n");
    }
}
