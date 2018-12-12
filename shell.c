#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pwd.h>

#include "shell.h"
// parse args v2
void parse_args(char *line, char **argv) {
    //Remove whitespace in front of input
    while (*line == ' ' || *line == '\t') {
        line++;
    }
    char *prev = NULL;
    int index = 0;

    while (line) {
        prev = line;
        strsep(&line, " ");
        if (*prev) {
            argv[index] = prev;
            index++;
        }
    } argv[index] = NULL; // add ending null
}

int execute(char **argv) {
    int status;
    int child = fork();

    if (child < 0) {
        // fork child to do work ;-;
        printf("Error: %s\n", strerror(errno));
        exit(1);
    } else if (child == 0) {
        // child: execute command
        if (execvp(*argv, argv) < 0) {
            // printf("%s\n", strerror(errno));
            return 0;
        }
    } else {
        // parent: wait till complete
        wait(&status);
    } return 1;
}

int executeSpecial(char *first, char *second, char *special) {
    char *argv[100];
    // stdout
    if (strcmp(special, ">") == 0) {
        int fd = open(second, O_WRONLY | O_TRUNC | O_CREAT, 0666);
        if (fd == -1) {
            printf("%s\n", strerror(errno));
            return 0;
        }
        int copy = dup(1);
        dup2(fd,1);
        parse_args(first, argv);
        int status = execute(argv);
        dup2(copy,1);
        close(fd);
        return status;
    }
    // put contents in stdin
    else if (strcmp(special, "<") == 0) {
        int fd = open(second, O_RDONLY);
        if (fd == -1) {
            printf("%s\n", strerror(errno));
            return 0;
        }
        int copy = dup(0);
        dup2(fd,0);
        parse_args(first, argv);
        int status = execute(argv);
        dup2(copy,0);
        close(fd);
        return status;
    }
    // piping
    else if (strcmp(special, "|") == 0){
        int fds[2];
        pipe(fds);

        parse_args(first, argv);
        char **args0 = argv;
        parse_args(second, argv);
        char **args1 = argv;

        pid_t f = fork();

        if (f < 0){
            printf("%s\n", strerror(errno));
        }
        if (f == 0){
            close(fds[0]);
            dup2(fds[1], STDOUT_FILENO);
            int error = execute(args0);
            if (error == -1)
            printf("%s\n", strerror(errno));
        }
        else{
            int cpid = wait(NULL);
            close(fds[1]);
            dup2(fds[0], STDIN_FILENO);
            int error = execute(args1);
            if (error == -1)
            printf("%s\n", strerror(errno));
        }
    }
    return 0;
}

// parse args v3 (special characters)
void parse_special(char *line, char **argv) {
    char *special;
    char *first;
    char *second;
    char *temp = line;
    //Remove whitespace in front of input
    while (*line == ' ' || *line == '\t') {
        line++;
    } while (*line != '\0') {
        if (*line == '<')
        special = "<";
        if (*line == '>')
        special = ">";
        if (*line == '|')
        special = "|";

        line++;
    }
    // printf("special char: %s\n", special);

    char *found;
    // printf("Original string: '%s'\n", temp);
    first = strsep(&temp, special);
    // printf("first: %s\n", first);
    second = strsep(&temp, special);
    // printf("second: %s\n", second);

    int status = executeSpecial(first, second, special);

    // printf("\nend\n");
}

// return 0 if special, 1 if not
int hasSpecial(char *current) {
    while (*current != '\0') {
        if (*current == '<' || *current == '>' || *current == '|') {
            return 0;
        } current++;
    } return 1;
}

void getDir(char out[]) {
    char path[1024];
    if (getcwd(path, 1024) == NULL){
        printf("Can't get directory\n");
        exit(1);
    } char *home = getenv("HOME");

    // if cwd is subdir of home
    if (strstr(path, home)){
        strcpy(out, "~");
        strcat(out, path + strlen(home));
    }
    // if cwd not in home
    else{
        strcpy(out, path);
    }
}

void doEverything() {
    int status;
    char line[500]; // input line
    char *current; // current command to execute
    char *argv[100]; // stores arguments
    char host_name[64]; // hostname
    gethostname(host_name,64);
    char cwd[1024]; // store cwd
    getDir(cwd);

    while (1) {
        // char * pter = malloc(sizeof(size));
        // buf = getcwd(pter, sizeof(size));
        // printf("path: %s\n", buf);
        // cmd line prompt
        if (isatty(STDIN_FILENO)) {
            printf("[%s]%s$ ",host_name, cwd);
        }
        gets(line);
        if (strcmp(line, "exit") == 0) {
            exit(0);
        }

        // run first part of semicolons
        char *args = line;
        while ((current = strsep(&args, ";")) != NULL && strcmp(current, "") != 0 && strcmp(current, " ")) {
            // printf("---------------another one-------------------\n\n");
            // printf("cmd: %s\n", current);
            if (hasSpecial(current)) {
                // printf("not special\n");
                // printf("\n");
                parse_args(current, argv);
                status = 1;
            } else {
                parse_special(current, argv);
                break;
            }
            // char *string, *found;
            // string = strdup("hello.txt<temp.txt");
            // printf("Original string: '%s'\n", string);
            // while((found = strsep(&string,"<")) != NULL )
            // printf("%s\n",found);

            // ----- IF CD ----- //
            if (strcmp(argv[0], "cd") == 0) {
                // no args(home dir)
                if (argv[1] == NULL || strcmp(argv[1], "~") == 0){
                    char * home;
                    chdir((home = getenv("HOME")));
                }
                // 1 arg
                else if (argv[2] == NULL && argv[1] != NULL){
                    if (chdir(argv[1]) != 0){
                        printf("%s\n", strerror(errno));
                    }
                }
                // error if more than 1 arg
                else {
                    printf("Error: cd: Too many arguments\n");
                }
                // refresh cwd
                getDir(cwd);
            }
            // char temp[50];
            // strcpy(temp, (char *) argv[1]);
            // printf("<%s>\n", temp);
            // printf("argv: %s\n", *argv);
            // if exit exit, otherwise execute

            // ----- IF EXIT ----- //
            // printf("%s\n", argv[0]);
            // if (strcmp(argv[0], "exit") == 0 || current == NULL) {
            //     exit(0);
            // }

            // printf("-------executing:%s---------\n", current);
            // printf("special?: %d\n", hasSpecial(current));
            if (status) {
                execute(argv);
            }
        }
    }
}
int main() {
    doEverything();
    return 0;
}
