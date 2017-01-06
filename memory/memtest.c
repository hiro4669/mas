#include <stdio.h>

#include "MEM.h"

int main(void) {
    printf("memtest\n");
    test();
    MEM_malloc(10);
    return 0;
}
