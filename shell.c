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
    char **args = calloc(sizeof(char *), 10);
    char *token;
    printf("ORIGINAL STRING:\n%s\n", line);
    while ((token = strsep(&line, ";")) != NULL) {
        printf("parsed: %s\n", token);
    }
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

void getDir(char out[]){
    char path[1024] = "";
    if (getcwd(path, 1024) == NULL){
        printf("Can't get directory\n");
        exit(1);
    } char *home = getenv("HOME");

    // if cwd is subdir of home
    if (strstr(path,home)){
        strcpy(out,"~");
        strcat(out,path+strlen(home));
    }
    // if cwd not in home
    else{
        strcpy(out,path);
    }
}

// int main() {
//     // store input
//     char line[500];             
//     // args
//     char *argv[100];              
//     char host_name[64];
//     gethostname(host_name,64);
//     char path[FILENAME_MAX];
//     getcwd(path, FILENAME_MAX);
//     while (1) {               
//         // char * pter = malloc(sizeof(size));
//         // buf = getcwd(pter, sizeof(size));
//         // printf("path: %s\n", buf);
//         // cmd line prompt    
//         printf("[%s]%s$ ",host_name, path);
//         // getPath();
//         gets(line);
//         parse(line, argv);       
//         // if exit exit, otherwise execute
//         if (strcmp(argv[0], "exit") == 0)  
//         exit(0);          
//         execute(argv);        
//         printf("\n");
//     }
// }

int main() {
    // store input
    char line[500];             
    char *current;
    
    char *argv[100];              
    char host_name[64];
    gethostname(host_name,64);
    char cwd[1024];
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
        current = strsep(&args, ";");
        printf("temp: %s\n", current);
        current = strsep(&args, ";");
        printf("next one: %s\n", current);
        parse(line, argv);       
        // if exit exit, otherwise execute
        if (strcmp(argv[0], "exit") == 0)  
            exit(0);          
        execute(argv);        
        printf("\n");
    } return 0;
}
