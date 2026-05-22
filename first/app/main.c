#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

#define SYS_get_hw_misa 450 

int main() {
    unsigned long misa_val = 0;
    
    printf("[App] request  MISA through custom syscall...\n");

    long res = syscall(SYS_get_hw_misa, &misa_val);

    if (res != 0) {
        return 1;
    }

    printf("MISA: 0x%lx\n\n", misa_val);
    
    
    
    int mxl = (misa_val >> 62) & 0x3;
    if (mxl == 1) printf("32");
    else if (mxl == 2) printf("64");
    else if (mxl == 3) printf("128");
    else printf("Unknown");

    printf("\nExtentions:\n");
    for (int i = 0; i < 26; i++) {
       
        if (misa_val & (1UL << i)) {
            char ext = 'A' + i;
            printf(" - [%c] : ", ext);
            switch (ext) {
                case 'I': printf("Base Integer Instruction Set\n"); break;
                case 'M': printf("Integer Multiplication and Division\n"); break;
                case 'A': printf("Atomic Instructions\n"); break;
                case 'F': printf("Single-Precision Floating-Point\n"); break;
                case 'D': printf("Double-Precision Floating-Point\n"); break;
                case 'C': printf("Compressed Instructions\n"); break;
                case 'V': printf("Vector Operations\n"); break;
                case 'S': printf("Supervisor Mode Implemented\n"); break;
                case 'U': printf("User Mode Implemented\n"); break;
                default:  printf("Other/Reserved Extension\n"); break;
            }
        }
    }

    return 0;
}
