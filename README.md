# Shell project: Xiaojie Li

## Implemented Features:
* shows user id and cwd in the prompt
* runs all basic bash commands(ls, pwd, echo, etc) with args and allows for spaces
* cd + exit, including cd into root
* semi colon separate/support for executing multiple programs in a row
* did I mention spacing removal!?
* redirection with ">" and "<"
* single piping

## Failed Features:
* didn't get to work on double pipes, tab completion, or bash command history

## Bugs/Quirks:
* piping only will work with simple commands such as `ls` and `wc`
* cannot use `<` or `>` in the same line or it'll crash

## Functions:
```c
char **parse_args(char *, char *);

void runAll(char *);

char cdExit(char *);

char *get_args();

char inputExec(char *);

void regularExec(char *);

char pipeExec(char *);

char outputExec(char *);

char *trim(char *);

void runSingle(char *);
```
