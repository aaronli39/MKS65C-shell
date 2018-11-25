#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pwd.h>

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

void getPath() {
    char *ret = calloc(sizeof(char *), 100);
    char path[FILENAME_MAX];
    getcwd(path, FILENAME_MAX);
    char temp[1000];
    strcpy(temp, path);
    printf("\n%d\n", temp[5]);
    int index = 0;
}

void main(void) {
    // store input
    char line[500];             
    // args
    char *argv[100];              
    char host_name[64];
    gethostname(host_name,64);
    char path[FILENAME_MAX];
    getcwd(path, FILENAME_MAX);
    while (1) {               
        // char * pter = malloc(sizeof(size));
        // buf = getcwd(pter, sizeof(size));
        // printf("path: %s\n", buf);
        // cmd line prompt    
        printf("[%s]%s$ ",host_name, path);
        getPath();
        gets(line);
        parse(line, argv);       
        // if exit exit, otherwise execute
        if (strcmp(argv[0], "exit") == 0)  
        exit(0);          
        execute(argv);        
        printf("\n");
    }
}

// int main(){
//   char * input;
//   char ** args;
//   int status = 1;
//   char * username = getpwuid(getuid())->pw_name;
//   char host_name[64];
//   gethostname(host_name,64);

//   while(status){
//     printf("%s@%s: $ ",username,host_name);
//     input = read_input();
//     char * cur_input = input;
//     strsep(&input,";");
//     while (cur_input){
//       //printf("Your input now is cur:%s or input:%s\n",cur_input,input);
//       args = parse_args(cur_input);
//       //print_list(args);
//       status = execute_args(args);
//       cur_input = input;
//       strsep(&input,";");
//     }
//   }
//   return 0;
// }
