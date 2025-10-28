#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Usage: sixfive <file>\n");
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd < 0){
        printf("sixfive: cannot open %s\n", argv[1]);
        exit(1);
    }

    char buf;
    int num = 0;
    int in_number = 0;
    const char *separators = " -\r\t\n./,";
    
    while(read(fd, &buf, 1) > 0){
        if(buf >= '0' && buf <= '9'){
            // We're reading a digit
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
                // End of number, check if divisible by 5 or 6
                if(num % 5 == 0 || num % 6 == 0){
                    printf("%d\n", num);
                }
                num = 0;
                in_number = 0;
            } else if(in_number && !is_sep){
                // Non-separator after number, reset
                num = 0;
                in_number = 0;
            }
        }
    }
    
    // Check last number if file ends with a number
    if(in_number && (num % 5 == 0 || num % 6 == 0)){
        printf("%d\n", num);
    }

    close(fd);
    exit(0);
}
