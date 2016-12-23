/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdint.h>
#include "mem.h"


typedef union {
    long l_dummy;
    double d_dummy;
    void *p_dummy;
} Align;


#define MARK_SIZE (4)

typedef struct {
    int size;
    char *filename;
    int line;
    Header *prev;
    Header *next;
    uint8_t mark[MARK_SIZE];    
} HeaderStruct;


#define ALIGN_SIZE (sizeof(Align)) /* 8 */
#define revalue_up_align(val)  ((val) ? (((val) - 1) / ALIGN_SIZE + 1) : 0)
#define HEADER_ALIGN_SIZE      (revalue_up_align(sizeof(HeaderStruct)))
#define MARK (0xCD)


union Header_tag {
    HeaderStruct s;
    Align u[HEADER_ALIGN_SIZE];
};

void test() {
    int val;
    void *p;
    
    printf("test\n");
    printf("ALIGN_SIZE = %d\n", (int)ALIGN_SIZE); // 8
    printf("HeaderStruct size = %d\n", (int)sizeof(HeaderStruct)); // 48
    printf("HEADER_ALIGN_SIZE = %d\n", (int)HEADER_ALIGN_SIZE);  // 6
    printf("u size = %d\n", (int)(sizeof(Align) * HEADER_ALIGN_SIZE));
    printf("long size = %d\n", (int)sizeof(long));
    printf("double size = %d\n", (int)sizeof(double));
    printf("void*  size = %d\n", (int)sizeof(p));
    printf("int    size = %d\n", (int)sizeof(int));    
    
    val = sizeof(HeaderStruct);
    val = 0;
    if (val) {
        printf("val = %d\n", val);
        printf("val2= %d\n", (val - 1) / 8 );
        
    } else {
        printf("boooo\n");
    }
    
}