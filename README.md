# MKS65C-shell: Xiaojie(Aaron) Li

## Implemented Features:
* shows user id and cwd in the prompt
* runs all basic bash commands(ls, pwd, echo, etc) with args and allows for spaces
* cd + exit, including cd into root
* semi colon separate/support for executing multiple programs in a row
* redirection with ">" and "<"
* single piping

## Failed Features:
* N/A

## Bugs/Quirks:
* for some reason my program crashes if while using semicolons, a pipe or redirection is executed first, followed by another standard program. For this reason, I made my program break from the while loop after executing a "special command," or a command that has a pipe or redirection in it. If you wish to using pipe/redirection, and chain it with multiple commands(semicolons), make sure that it is the last command
* piping will only work if there is no space after the "|": eg: ```ls |wc``` will work but ```ls | wc``` will not.
* due to file naming and spaces in file names, and the way I parsed them, doing ```ls > temp.txt``` will put the correct ls information but into a file called ```' temp.txt'``` instead of ```temp.txt```
* you can't use redirections in the same line
* for some reason pipes dont work on Stuy computers. Eg: ```ls |wc``` will not work(the program seems to be stuck in a loop)

## Functions:
```c
void parse_args(char *, char **);

int execute(char **);

int executeSpecial(char *, char *, char *);

void parse_special(char *, char **);

int hasSpecial(char *);

void getDir();

void doEverything();
```
