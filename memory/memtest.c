#include <stdio.h>

#include "MEM.h"

int main(void) {
    char *ptr, *ptr2;
    int i;
    printf("memtest\n");
    test();    
    ptr = (char*)MEM_malloc(10);
    for (i = 0; i < 10; ++i) {
//        *ptr++ = 0xaa;
        ptr[i] = 0xaa;
    }
//    MEM_malloc(11);
//    MEM_dump_memory();
    MEM_free(ptr);
    MEM_dump_memory();
    

    MEM_Storage storage = MEM_open_storage(3);
    ptr = MEM_storage_malloc(storage, 10);
    for (i = 0; i < 10; ++i) {
        ptr[i] = 0xaa;
    }
    ptr2 = MEM_storage_malloc(storage, 10);
    for (i = 0; i < 10; ++i) {
        ptr2[i] = 0xbb;
    }
    
    MEM_dump_memory();
    MEM_dispose(storage);
    MEM_dump_memory();
    
    return 0;
}
