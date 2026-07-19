#include "viksh.h"

void Getcwd(char *buf,size_t size){
    if(NULL == getcwd(buf,size)){
        perror(RED"getcwd FAILED"RST);

    }
}

char *builtin_str[]={
    "cd","pwd","ls","mkdir","rmdir","touch","rm","help","exit"
};

int(*builtin_func[])(char **)={
    &viksh_cd,
    &viksh_pwd,
    &viksh_ls,
    &viksh_mkdir,
    &viksh_rmdir,
    &viksh_touch,
    &viksh_rm,
    &viksh_help,
    &viksh_exit
};

int viksh_num_builtins(void){
    return sizeof(builtin_str)/ sizeof(char*);
}

void redraw_line(char *buf)
{
    printf("\33[2K\r"); // erase line
    printf(G "viksh $> " RST);
    printf("%s", buf);
    fflush(stdout);
}

char* current_token(char* buf){
    char* token= strrchr(buf,' ');
    return token ? token+1 : buf;
}

void find_builtin_matches(char* buf, size_t *bufsize){
    char* token = current_token(buf);
    for(int i=0;i<viksh_num_builtins();i++){
        if(strncmp(builtin_str[i],token,strlen(token))==0){
            strcpy(token, builtin_str[i]);
            *bufsize = strlen(buf);
            redraw_line(buf);
        }
    }
}

void find_path_matches(char* buf, size_t *bufsize){
    char* token = current_token(buf);
    char* matches[MAX_MATCHES];
    int match_count=0;

    char *path=getenv("PATH");
    char *path_actual=strdup(path); //path gets the pointer only, path_actual stores the string
    if(!path_actual) return;
    
    char* dir = strtok(path_actual,":");
    while(dir){
        DIR *d=opendir(dir);
        if(!d){
            dir=strtok(NULL,":");
            continue;
        }
        struct dirent* entry;
        while(entry=readdir(d)){
            if(strncmp(entry->d_name,token,strlen(token))==0){
                char fullpath[PATH_MAX];
                snprintf(fullpath,sizeof(fullpath),"%s/%s",dir,entry->d_name);
                if(access(fullpath,X_OK)==0){
                    //it is an executable
                    if(match_count < MAX_MATCHES){
                        matches[match_count++]= strdup(entry->d_name);
                    }
                }
            }
        }
        closedir(d);
        dir=strtok(NULL,":");
    }
    if(match_count==1){
        strcpy(token,matches[0]);
        strcat(buf," ");
        *bufsize=strlen(buf);
        redraw_line(buf);
    }
    if(match_count>1){
        //several matches printed in the terminal
        printf("\n");
        for(int i=0;i<match_count;i++){
            printf("%s  ",matches[i]);
        }
        printf("\n");
        redraw_line(buf);
    }

    for(int i=0;i<match_count;i++){
        free(matches[i]);
    }
    free(path_actual);
}