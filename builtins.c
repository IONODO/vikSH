#include "viksh.h"



int viksh_pwd(char**){
    char cwd[BUFSIZ];
    Getcwd(cwd,sizeof(cwd));
    printf("%s\n",cwd);
    return 1;
}

int viksh_cd(char **args){
    if(args[1]==NULL){
        fprintf(stderr,RED"viksh: EXPECTED ARGUMENT TO \"cd\"\n"RST);
    } else{
        if(chdir(args[1])!=0){
            perror("viksh");
        }
    }
    return 1;
}

int viksh_help(char **args){
    printf("Welcome to vikSH\n");
    printf("Works like any other shell... maybe, its just very basic but cd and shi works\n");
    printf("Working functions rn: \n");
    for(int i=0;i<viksh_num_builtins();i++){
        printf("    %s\n",builtin_str[i]);
    }
    return 1;
}

int viksh_exit(char **args){
    return 0;
}

int viksh_ls(char **args){
    DIR *dir;
    struct dirent *entry;
    
    dir=opendir(".");
    if(dir==NULL){
        perror(RED"Unable to open directory"RST);
        return 0;
    }
    while((entry=readdir(dir))!=NULL){
        printf("%s\n",entry->d_name);
    }
    closedir(dir);
    return 1;
}

int viksh_mkdir(char **args){
    if(args[1]==NULL){
        fprintf(stderr,RED"viksh: EXPECTED ARGUMENT TO \"mkdir\" \n"RST);
    } else{
        if(mkdir(args[1],0755)==0){
            printf("Directory created successfuly!\n");
        } else{
            if(errno==EEXIST){
                fprintf(stderr,RED"viksh: DIRECTORY ALREADY EXISTS!\n"RST);
            } else{
                perror(RED"viksh: FAILED TO CREATE DIRECTORY\n"RST);
            }
        }
    }
    return 1;
}

int viksh_rmdir(char **args){
    if(args[1]==NULL){
        fprintf(stderr,RED"viksh: EXPECTED ARGUMENT TO \"rmdir\" \n"RST);
    } else{
        if(rmdir(args[1])==0){
            printf("Directory deleted successfully!\n");
        } else{
            if(errno==ENOTEMPTY){
                fprintf(stderr,RED"viksh: CANNOT DELETE A NON-EMPTY DIRECTORY\n"RST);
            } else if(errno==ENOENT){
                fprintf(stderr,RED"viksh: PATH DOES NOT EXIST\n"RST);
            } else if(errno==EACCES){
                fprintf(stderr,RED"viksh: ACCESS DENIED\n"RST);
            } else{
                perror(RED"viksh: FAILED TO DELETE DIRECTORY\n"RST);
            }
        }
    }
    return 1;
}

int viksh_touch(char **args){
    if(args[1]==NULL){
        fprintf(stderr,RED"viksh: EXPECTED ARGUMENT TO \"touch\" \n"RST);
    } else{
        int fd=open(args[1],O_CREAT | O_WRONLY,0644); //0644 means read/write for ownder, read for others
        if(fd==-1){
            fprintf(stderr,RED"viksh: ERROR CREATING FILE"RST);
        }
        close(fd);
        // if (utimensat(AT_FDCWD, args[1], NULL, 0) == -1) {
        // perror("Error updating timestamps");
        // return 1;
        // }
        printf("Successfully touched ;)\n");
    }
    return 1;
}

int viksh_rm(char **args){
    if(args[1]==NULL){
        fprintf(stderr,RED"viksh: EXPECTED ARGUMENT TO \"rm\" \n"RST);
    } else{
        if(remove(args[1])==0){
            printf("Successfully removed\n");
        } else if(errno==ENOENT){
                fprintf(stderr,RED"viksh: FILE DOES NOT EXIST\n"RST);
        } else{
            fprintf(stderr,RED"viksh: ERROR DELETING FILE\n"RST);
        }
    }
    return 1;
}