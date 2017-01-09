#include <stdio.h>

#include "MEM.h"

int main(void) {
    char *ptr;
    int i;
    printf("memtest\n");
    test();
    ptr = (char*)MEM_malloc(10);
    for (i = 0; i < 10; ++i) {
//        *ptr++ = 0xaa;
        ptr[i] = 0xaa;
    }
    MEM_malloc(11);
    
    
    MEM_dump_memory();
    MEM_free(ptr);
    MEM_dump_memory();
    return 0;
}
