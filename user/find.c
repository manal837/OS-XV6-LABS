#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

int match(char*, char*);

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

int
matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*'){
    return matchstar(re[0], re+2, text);
  }
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text)){
    return matchhere(re+1, text+1);
  }
  return 0;
}

int
matchstar(int c, char *re, char *text)
{
  do{
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}

void find(char *path, char *pattern, int exec_argc, char *exec_argv[]) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
    case T_FILE:
        {
            char *name = path;
            for(char *ptr = path; *ptr; ptr++){
                if(*ptr == '/'){
                    name = ptr + 1;
                }
            }
            
            if(match(pattern, name)){
                if(exec_argc > 0){
                    int pid = fork();
                    if(pid == 0){
                        char *argv[MAXARG];
                        int i;
                        for(i = 0; i < exec_argc; i++){
                            argv[i] = exec_argv[i];
                        }
                        argv[exec_argc] = path;
                        argv[exec_argc + 1] = 0;
                        exec(argv[0], argv);
                        fprintf(2, "find: exec %s failed\n", argv[0]);
                        exit(1);
                    } else if(pid > 0){
                        wait(0);
                    } else {
                        fprintf(2, "find: fork failed\n");
                    }
                } else {
                    printf("%s\n", path);
                }
            }
        }
        break;

    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0){
                continue;
            }
            find(buf, pattern, exec_argc, exec_argv);
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc < 3){
        printf("Usage: find <directory> <pattern> [-exec command...]\n");
        exit(1);
    }

    char *path = argv[1];
    char *pattern = argv[2];
    int exec_mode = 0;
    int exec_argc = 0;
    char *exec_argv[MAXARG];

    if(argc >= 4 && strcmp(argv[3], "-exec") == 0){
        exec_mode = 1;
        for(int i = 4; i < argc; i++){
            if(exec_argc >= MAXARG - 2){
                fprintf(2, "find: too many arguments for -exec\n");
                exit(1);
            }
            exec_argv[exec_argc++] = argv[i];
        }
        if(exec_argc == 0){
            fprintf(2, "find: -exec requires a command\n");
            exit(1);
        }
    }

    find(path, pattern, exec_mode ? exec_argc : 0, exec_argv);
    exit(0);
}
