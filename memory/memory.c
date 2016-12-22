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
    printf("test\n");
    printf("ALIGN_SIZE = %d\n", (int)ALIGN_SIZE);
    
    
}