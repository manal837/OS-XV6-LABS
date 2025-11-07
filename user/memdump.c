#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{
  char *p = data;
    
    for(int i = 0; fmt[i] != '\0'; i++) {
        switch(fmt[i]) {
            case 'i': {
                // Print 4 bytes as 32-bit integer
                int *val = (int*)p;
                printf("%d\n", *val);
                p += 4;
                break;
            }
            case 'p': {
                // Print 8 bytes as 64-bit integer in hex
                unsigned long *val = (unsigned long*)p;
                printf("%lx\n", *val);
                p += 8;
                break;
            }
            case 'h': {
                // Print 2 bytes as 16-bit integer
                short *val = (short*)p;
                printf("%d\n", *val);
                p += 2;
                break;
            }
            case 'c': {
                // Print 1 byte as ASCII character
                printf("%c\n", *p);
                p += 1;
                break;
            }
            case 's': {
                // Print string from pointer
                char **str_ptr = (char**)p;
                printf("%s\n", *str_ptr);
                p += 8;
                break;
            }
            case 'S': {
                // Print rest as null-terminated string
                printf("%s\n", p);
                return;
            }
            default:
                break;
        }
    }
}
