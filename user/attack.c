#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"
#include "kernel/stat.h"

int main() {
    // Allocate 64KB of memory
    char *mem = sbrk(65536);
    if(mem == (char*)-1) {
        printf("sbrk failed\n");
        exit(1);
    }
    
    // Search for strings and print them
    for(int i = 0; i < 65536 - 1; i++) {
        // Look for start of string (alphanumeric)
        if(((mem[i] >= 'a' && mem[i] <= 'z') || 
            (mem[i] >= 'A' && mem[i] <= 'Z') || 
            (mem[i] >= '0' && mem[i] <= '9')) &&
           ((mem[i+1] >= 'a' && mem[i+1] <= 'z') || 
            (mem[i+1] >= 'A' && mem[i+1] <= 'Z') || 
            (mem[i+1] >= '0' && mem[i+1] <= '9'))) {
            
            int start = i;
            int len = 0;
            
            // Measure the string length
            while(i < 65536 && 
                  ((mem[i] >= 'a' && mem[i] <= 'z') || 
                   (mem[i] >= 'A' && mem[i] <= 'Z') || 
                   (mem[i] >= '0' && mem[i] <= '9'))) {
                len++;
                i++;
            }
            
            // Print if it's a reasonable length for a secret
            if(len >= 6 && len <= 64) {
                write(1, &mem[start], len);
                write(1, "\n", 1);
            }
        }
    }
    
    sbrk(-65536);
    exit(0);
}
