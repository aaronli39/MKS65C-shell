#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"

char cwd[1024]; // store cwd

// getDir
// input: out[] is char array storing the cwd
// out: void, refreshes cwd
void getDir(char out[])
{
    char path[1024];
    if (getcwd(path, 1024) == NULL)
    {
        printf("Can't get directory\n");
        exit(1);
    }
    char *home = getenv("HOME");

    // if cwd is subdir of home
    if (strstr(path, home))
    {
        strcpy(out, "~");
        strcat(out, path + strlen(home));
    }
    // if cwd not in home
    else
    {
        strcpy(out, path);
    }
}

// trim
// input: char *to_trim is string you wish to trim
// out: the trimmed string
char *trim(char *to_trim)
{
    char *start = to_trim;
    while (*start == ' ' || *start == '\t')
    {
        start++;
    }
    // ending
    char *end = to_trim;
    while (*end)
    {
        end++;
    }
    end--;
    while (*end == ' ' || *end == '\t')
    {
        *end = 0;
        end--;
    }
    return start;
}

// parse_args(v3)
// input: char *line, *parse: line is input line, parse is special char
// output: ** with arguments of input line
char **parse_args(char *line, char *parse)
{
    char **args = calloc(sizeof(char *), 101);
    int index = 0;
    char *token;
    while (line != NULL && index < 100)
    {
        token = strsep(&line, parse);
        if (strcmp(token, ""))
        {
            args[index] = token;
            index++;
        }
    }
    return args;
}

// get_args()
// gets the flags of each command
char *get_args()
{
    char *args = calloc(sizeof(char), 1024);
    fgets(args, 1023, stdin);
    char *pter = strrchr(args, '\n');
    if (pter)
        *pter = 0;
    return args;
}

// runAll is an omega run for all the runs
// input: arg is line input
// output: void, runs everything
void runAll(char *arg)
{
    // parse on semis
    char *pter = strchr(arg, '\n');
    if (pter)
        *pter = 0;
    char **arg_array = parse_args(arg, ";");

    int index = 0;
    while (arg_array[index])
    {
        runSingle(arg_array[index]);
        index++;
    }
}

// runSingle
// input: line of command input
// out: void, runs appropriate functions depending on type
// of input
void runSingle(char *arg_line)
{
    char *pter = strchr(arg_line, '\n');
    if (pter)
        *pter = 0;
    // not cd or exit
    char *cd_line = calloc(sizeof(char), 1024);
    strcpy(cd_line, arg_line);
    if (cdExit(cd_line))
    {
        return;
    }
    // |
    char *pipe_line = calloc(sizeof(char), 1024);
    strcpy(pipe_line, arg_line);
    if (pipeExec(pipe_line))
    {
        return;
    }
    // <
    char *ie_line = calloc(sizeof(char), 1024);
    strcpy(ie_line, arg_line);
    if (inputExec(ie_line))
    {
        return;
    }
    // >
    char *oe_line = calloc(sizeof(char), 1024);
    strcpy(oe_line, arg_line);
    if (outputExec(oe_line))
    {
        return;
    }

    regularExec(arg_line);
}

// outputExec
// input: line of command
// output: return status
char outputExec(char *line)
{
    if (line[0] == '>')
    {
        printf("Error with >\n");
        return 1;
    }
    if (!strchr(line, '>'))
    {
        return 0;
    }
    char **args = parse_args(line, ">");
    if (!fork())
    {
        int index = 1;
        int fd;
        char *filename;
        while (args[index] && index < 101)
        {
            filename = trim(args[index]);
            fd = open(filename, O_WRONLY | O_CREAT, 0644);
            if (fd == -1)
            {
                printf("%s\n", strerror(errno));
                return 1;
            }
            index++;
        }
        dup2(fd, STDOUT_FILENO);
        char **commands = parse_args(args[0], " ");

        int error = execvp(commands[0], commands);
        if (error == -1)
        {
            printf("Error with: %s\n", args[0]);
            printf("%s\n", strerror(errno));
        }
        return 1;
    }
    else
    {
        int status;
        wait(&status);
        return 1;
    }
}

// pipeExec
// input: line of command
// out: if successfully executed
char pipeExec(char *line)
{
    // error
    if (line[0] == '|')
    {
        printf("| : Error with pipe\n");
        return 1;
    }
    if (!strchr(line, '|'))
    {
        return 0;
    }
    if (!fork())
    {
        int fds[2];
        pipe(fds);

        char **args = parse_args(line, "|");

        // left/right args
        char **left = parse_args(args[0], " ");
        char **right = parse_args(args[1], " ");

        if (fork())
        {
            close(fds[0]);
            dup2(fds[1], STDOUT_FILENO);
            execvp(left[0], left);
            return 1;
        }
        else
        {
            wait(NULL);
            close(fds[1]);
            dup2(fds[0], STDIN_FILENO);
            execvp(right[0], right);
            return 1;
        }
    }
    else
    {
        wait(NULL);
        return 1;
    }
    return 1;
}

// inputExec
// input: line of command for <
// output: whether or not ran successfully
char inputExec(char *line)
{
    if (line[0] == '<')
    {
        printf("Error with <\n");
        return 1;
    }
    if (!strchr(line, '<'))
    {
        return 0;
    }
    char **args = parse_args(line, "<");
    if (!fork())
    {
        int index = 1;
        int fd;
        char *filename;
        while (args[index] && index < 101)
        {
            filename = trim(args[index]);
            fd = open(filename, O_RDONLY);
            if (fd == -1)
            {
                printf("%s\n", strerror(errno));
                return 1;
            }
            index++;
        }
        dup2(fd, STDIN_FILENO);
        char **stuff = parse_args(args[0], " ");

        int error = execvp(stuff[0], stuff);
        if (error == -1)
        {
            printf("Error with arg: %s\n", args[0]);
            printf("%s\n", strerror(errno));
        }
        return 1;
    }
    else
    {
        int status;
        wait(&status);
        return 1;
    }
}

// regularExec
// input: line of input command
// out: none, runs the command
void regularExec(char *arg_line)
{
    char **arg_array = parse_args(arg_line, " ");
    int index = 0;
    if (!fork())
    {
        int error = execvp(arg_array[0], arg_array);
        if (error == -1)
        {
            printf("Error with %s: not a command\n", arg_array[0]);
        }
        return;
    }
    else
    {
        int status;
        wait(&status);
    }
}

// cdExit
// input: a line of input
// output: 1 or 0 for failure or success of execution
char cdExit(char *arg_line)
{
    char **arg_array = parse_args(arg_line, " ");
    if (!strcmp(arg_array[0], "exit"))
    {
        free(arg_array);
        exit(0);
    }
    else if (!strcmp(arg_array[0], "cd"))
    {
        // no args(home dir)
        if (arg_array[1] == NULL || strcmp(arg_array[1], "~") == 0)
        {
            char *home;
            chdir((home = getenv("HOME")));
        }
        // 1 arg
        else if (arg_array[2] == NULL && arg_array[1] != NULL)
        {
            if (chdir(arg_array[1]) != 0)
            {
                printf("%s\n", strerror(errno));
            }
        }
        // error if more than 1 arg
        else
        {
            printf("Error: cd: Too many arguments\n");
        } // refresh cwd
        getDir(cwd);
        return 1;
    }
    return 0;
}

// simply prints my prompt
void print_prompt()
{
    if (isatty(STDIN_FILENO))
    {
        char host_name[64]; // hostname
        gethostname(host_name, 64);
        getDir(cwd);
        printf("[%s]%s$ ", host_name, cwd);
    }
}

// run everything
int main()
{
    char host_name[64]; // hostname
    gethostname(host_name, 64);
    while (1)
    {
        // refresh the dir
        if (isatty(STDIN_FILENO))
        {
            getDir(cwd);
            printf("[%s]%s$ ", host_name, cwd);
        }
        char *line = get_args();

        runAll(line);
    }
}