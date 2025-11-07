#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void process_file(char *filename) {
    int fd = open(filename, O_RDONLY);
    if(fd < 0){
        printf("sixfive: cannot open %s\n", filename);
        return;
    }

    char buf;
    int num = 0;
    int in_number = 0;
    const char *separators = " -\r\t\n./,";
    
    while(read(fd, &buf, 1) > 0){
        if(buf >= '0' && buf <= '9'){
            num = num * 10 + (buf - '0');
            in_number = 1;
        } else {
            // Check if this character is a separator
            int is_sep = 0;
            for(int i = 0; separators[i]; i++){
                if(buf == separators[i]){
                    is_sep = 1;
                    break;
                }
            }
            
            if(in_number && is_sep){
                if(num % 5 == 0 || num % 6 == 0){
                    printf("%d\n", num);
                }
                num = 0;
                in_number = 0;
            } else if(in_number && !is_sep){
                num = 0;
                in_number = 0;
            }
        }
    }
    
    if(in_number && (num % 5 == 0 || num % 6 == 0)){
        printf("%d\n", num);
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("Usage: sixfive <file1> [file2] ...\n");
        exit(1);
    }

    for(int i = 1; i < argc; i++){
        process_file(argv[i]);
    }
    
    exit(0);
}
