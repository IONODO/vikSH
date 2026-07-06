//welcome to vikSH, more like weakSH cause this shi gonna be basic af
//let's start baby!

#include "viksh.h"

char* viksh_read_line(void){
    char *buf=NULL;
    size_t bufsize=0;
    //char cwd[BUFSIZ];
    buf=NULL;

    // Getcwd(cwd,sizeof(cwd));
    printf(G"viksh $> "RST);

    if(getline(&buf, &bufsize, stdin) == -1){
        if(feof(stdin)){
            printf(RED" EOF "RST);
            exit(EXIT_SUCCESS);
        } else{
            perror(RED"readline failed"RST);
            exit(EXIT_FAILURE);
        }
    }
    return buf;
}

char** viksh_split_line(char *line){
    int bufsize = LSH_TOK_BUFSIZE;
    int position=0;
    char **tokens = malloc(bufsize*sizeof(char*));
    char *token;

    if(!tokens){
        fprintf(stderr,RED"VIKSH: ALLOCATION ERROR"RST);
        exit(EXIT_FAILURE);
    }

    token = strtok(line,LSH_TOK_DELIM);
    while(token!=NULL){
        tokens[position]=token;
        position++;

        if(position >= bufsize){
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens,bufsize*sizeof(char*));
            if(!tokens){
                fprintf(stderr,RED"viksh: ALLOCATION ERROR"RST);
                exit(EXIT_FAILURE);
            }
        }
        token=strtok(NULL,LSH_TOK_DELIM);
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
        //printf("v> ");
        line=viksh_read_line();
        args=viksh_split_line(line);
        status=viksh_execute(args);

        free(line);
        free(args);
    } while(status);
}

int main(int argc, char **argv){ //these are the standard parameters pased to main to handle command line prompts
    viksh_loop();

    return EXIT_SUCCESS;
}