#ifndef VIK_SH
#define VIK_SH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <termios.h>
#include <linux/limits.h>

#define VIKSH_LINE_BUFSIZE 1024
#define VIKSH_TOK_BUFSIZE 64
#define VIKSH_TOK_DELIM " \t\r\n\a"
#define HISTORY_SIZE 100
#define MAX_MATCHES 250

//colours
#define Y   "\033[1;33m"
#define G   "\033[1;32m"
#define C   "\033[1;36m"
#define RED "\033[1;31m"
#define RST "\033[0m"

//utils.c functions, also has builtins
//for all future builtins, i just declare here, write the function in utils.c and leave the main viksh.c file untouched
//(hopefully its that simple)
void Getcwd(char *, size_t);
int viksh_pwd(char**);
int viksh_cd(char **);
int viksh_help(char** );
int viksh_exit(char **);
int viksh_ls(char **);
int viksh_mkdir(char **);
int viksh_rmdir(char **);
int viksh_touch(char **);
int viksh_rm(char **);
int viksh_num_builtins(void);

void redraw_line(char*);
void find_builtin_matches(char*,size_t*);
void find_path_matches(char*, size_t*);

//declaration of arrays for builtin commands and the functions they call respectively
extern char *builtin_str[];
extern int(*builtin_func[])(char **);

#endif