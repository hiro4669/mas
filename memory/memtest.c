#include <stdio.h>

#include "MEM.h"
#include "../info.h"

int main(void) {
    char *ptr, *ptr2;
    LocalInfo *l_info;
    int i;
    test();    
    ptr = (char*)MEM_malloc(10);
    for (i = 0; i < 10; ++i) {
        ptr[i] = 0xaa;
    }

    MEM_dump_memory();
    MEM_realloc(20, ptr);
//    MEM_free(ptr);
//    MEM_dump_memory();
    /*
    l_info = (LocalInfo*)MEM_malloc(sizeof(LocalInfo));
    l_info->line_number = 0xff;
    MEM_dump_memory();
    MEM_free(l_info);
    */
    

    printf("------------------------------------\n");

    /*
    MEM_Storage storage = MEM_open_storage(0);
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
     */
    return 0;
}
