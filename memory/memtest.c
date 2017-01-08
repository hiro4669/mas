#include <stdio.h>

#include "MEM.h"

int main(void) {
    printf("memtest\n");
    test();
    MEM_malloc(10);
    MEM_dump_memory();
    return 0;
}
