
#include "get_path.h"

int pid;
char *prompt;
int sh( int argc, char **argv, char **envp );
void sighand( int sig);
char *which(char **args, struct pathelement *p);
char * where(char **args, struct pathelement *pathlist);
void exit_shell();
void pwdirect();
void changedir(char ** args);
int list (char **args );
void printpid();
void kill_pid(char **args);
void prompt_(char ** args);
void printenv(char **args, char **envp);
void setenv_(char **args, char **envp, struct pathelement *pathlist);

#define PROMPTMAX 32
#define MAXARGS 10


char *input_cmd[12];
