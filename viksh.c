//welcome to vikSH, more like weakSH cause this shi gonna be basic af
//let's start baby!

#include "viksh.h"

//global declarations because they get used pretty early on
static struct termios original_termios;
char* history[HISTORY_SIZE];
int history_count=0;

void initTerminal(void)
{
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1)
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
}

void enableRawMode(void){
    struct termios raw = original_termios;
    raw.c_lflag &= ~(ICANON | ECHO); //ICANON=canonical input
    raw.c_iflag &= ~IXON; //this is for ctrl+s and ctrl+q
    //raw.c_oflag &= ~(OPOST); //output post processing
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); 
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        perror("tcsetattr");
}

void disableRawMode(void){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1)
        perror("tcsetattr");
}

void viksh_autocomplete(char *buf, size_t *bufsize){

    find_builtin_matches(buf,bufsize);
    find_path_matches(buf,bufsize);
}

char* viksh_read_line_raw(void){
    char* buf=malloc(VIKSH_LINE_BUFSIZE);
    buf[0]='\0';
    if(!buf){
        perror("malloc failed!!");
        exit(EXIT_FAILURE);
    }
    size_t bufsize=0;
    int history_index = history_count;
    printf(G"viksh $> "RST);
    fflush(stdout);

    while(1){
        char c;
        ssize_t n =read(STDIN_FILENO, &c, 1);
        
        if(n<=0) continue;
        switch((unsigned char)c){
            //enter key pressing
            case '\r':
            case '\n':
                buf[bufsize]='\0';
                history[history_count++] = strdup(buf);
                if (history_count == HISTORY_SIZE) {
                    free(history[0]);
                    memmove(history, history + 1, sizeof(char *) * (HISTORY_SIZE - 1));
                    history_count--;
                }
                write(STDOUT_FILENO, "\r\n", 2);
                return buf;
            //tab key pressing
            case '\t':
                viksh_autocomplete(buf,&bufsize);
                break;
            //backspace key
            case 127:
            case '\b':
                if(bufsize>0){
                    bufsize--;
                    write(STDOUT_FILENO, "\b \b",3);
                }
                break;
            //arrow keys
            case 27:
                char seq[2];
                read(STDIN_FILENO, &seq[0], 1);
                read(STDIN_FILENO, &seq[1], 1);
                if(seq[0]=='[' && seq[1]=='A'){
                    if(history_count>0 && history_index>0){
                        history_index--;
                    }
                    strcpy(buf,history[history_index]);
                    bufsize=strlen(buf);
                    redraw_line(buf);
                } else if(seq[0]=='[' && seq[1]=='B'){
                    if(history_index < history_count-1){
                        history_index++;
                        strcpy(buf,history[history_index]);
                        bufsize=strlen(buf);
                    } else{
                        history_index = history_count;
                        buf[0]='\0';
                        bufsize=0;
                    }
                    redraw_line(buf);
                }
                break;
            default:
                if(bufsize<VIKSH_LINE_BUFSIZE-1){
                    buf[bufsize++] = c;
                    buf[bufsize]='\0';
                }
                write(STDOUT_FILENO, &c, 1);
        }
    }

}

//legacy function, let it be
char* viksh_read_line(void){
    printf(G"viksh $> "RST);
    char *buf=NULL;
    size_t bufsize=0;
    //buf=NULL;
    
    //viksh_read_line_raw();

    // if(getline(&buf, &bufsize, stdin) == -1){
    //     if(feof(stdin)){
    //         printf(RED" EOF "RST);
    //         exit(EXIT_SUCCESS);
    //     } else{
    //         perror(RED"readline failed"RST);
    //         exit(EXIT_FAILURE);
    //     }
    // }
    // return buf;
}

char** viksh_split_line(char *line){
    int bufsize = VIKSH_TOK_BUFSIZE;
    int position=0;
    char **tokens = malloc(bufsize*sizeof(char*));
    char *token;

    if(!tokens){
        fprintf(stderr,RED"VIKSH: ALLOCATION ERROR"RST);
        exit(EXIT_FAILURE);
    }

    token = strtok(line,VIKSH_TOK_DELIM);
    while(token!=NULL){
        tokens[position]=token;
        position++;

        if(position >= bufsize){
            bufsize += VIKSH_TOK_BUFSIZE;
            tokens = realloc(tokens,bufsize*sizeof(char*));
            if(!tokens){
                fprintf(stderr,RED"viksh: ALLOCATION ERROR"RST);
                exit(EXIT_FAILURE);
            }
        }
        token=strtok(NULL,VIKSH_TOK_DELIM);
    }

    tokens[position]=NULL;
    return tokens;
}

int viksh_launch(char** args){
    pid_t pid,wpid;
    int status;

    pid=fork();
    if(pid==0){
        //child process
        if(execvp(args[0],args)==-1){
            perror("viksh");
        }
        exit(EXIT_FAILURE);
    } else if(pid<0){
        //this is a fork(lol) incase error occurs
        perror("viksh");
    } else{
        do{
            wpid=waitpid(pid,&status,WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

//final execute function to run the parsed command from shell and finally run it

int viksh_execute(char** args){
    if(args[0]==NULL){
        return 1;
    }
    for(int i=0;i<viksh_num_builtins();i++){
        if(strcmp(args[0],builtin_str[i])==0){
            return(*builtin_func[i])(args);
        }
    }

    return viksh_launch(args);
}

void viksh_loop(){
    char *line;
    char **args;
    int status;

    do{
        enableRawMode();
        line=viksh_read_line_raw();
        disableRawMode();
        args=viksh_split_line(line);
        status=viksh_execute(args);

        free(line);
        free(args);
    } while(status);
}

int main(void){ 
    initTerminal();
    atexit(disableRawMode);
    viksh_loop();
    // at first i used int argc, char **argv as passed parameters, but it was static, i type and press enter
    // and only then it would execute, no autocompletion, no syntax highlighting, nothing fancy was possible
    // so the next approach was to make a live reading mode, raw mode in shells and vim, to be more dynamic
    // also the reason raw mode is set, then viksh_loop runs is so that the shell's behaviour doesnt interfer with another program called like say vim

    return EXIT_SUCCESS;
}