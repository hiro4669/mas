#include <stdio.h>

#include "MEM.h"
#include "../info.h"

int main(void) {
    char *ptr, *ptr2;
    LocalInfo *l_info;
    int i;
    test();    
    /*
    ptr = (char*)MEM_malloc(10);
    for (i = 0; i < 10; ++i) {
        ptr[i] = 0xaa;
    }
    */
    ptr2 = (char*)MEM_malloc(20);

    MEM_dump_memory();
    fprintf(stderr, "do realloc\n");
    ptr2 = MEM_realloc(30, ptr2);
    for (i = 0; i < 30; ++i) {
        ptr2[i] = 0xab;
    }
    MEM_dump_memory();
//    MEM_free(ptr);
//    fprintf(stderr, "--dump--\n");
//    MEM_dump_memory();
    MEM_free(ptr2);
    fprintf(stderr, "--dump--\n");
    MEM_dump_memory();
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
