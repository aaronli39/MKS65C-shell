#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pwd.h>

void parse_args(char *line, char **argv) {
    //Remove whitespace in front of input
    while (*line == ' ' || *line == '\t'){
        line++;
    }
    char *prev = NULL;
    int index = 0;

    while (line) {
        prev = line;
        strsep(&line, " ");
        int num_special = 0;
        //Checks for special characters and adds them into the string array
        // num_special += sep_special(old,&output,">>",&arg_index);
        // num_special += sep_special(old,&output,">",&arg_index);
        // num_special += sep_special(old,&output,"<",&arg_index);
        // num_special += sep_special(old,&output,"|",&arg_index);
        //If there was not anything special about the current arg, then just add it
        if (num_special < 1 && *prev){
            argv[index] = prev;
            index++;
        }
    } argv[index] = NULL; // add ending null
}

void parse(char *line, char **argv) {
    // if not end of line
    while (*line != '\0') {
        // replace white spaces with 0
        while (*line == ' ' || *line == '\t')
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
            // printf("%s\n", strerror(errno));
            exit(1);
        }
    } else {
        // parent: wait till complete
        wait(&status);
    }
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

int main() {
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
        printf("[%s]%s$ ",host_name, cwd);
        gets(line);

        // run first part of semicolons
        char *args = line;
        while ((current = strsep(&args, ";")) != NULL && strcmp(current, "") != 0) {
            // printf("-------executing:%s---------\n", current);
            parse_args(current, argv);
            printf("xd: %d\n", ' ');
            // if cd
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
            if (strcmp(argv[0], "exit") == 0 || current == NULL) {
                exit(0);
            } execute(argv);
        }
    } return 0;
}
