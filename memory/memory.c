/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

static struct MEM_Controller_tag st_default_controller = {
    NULL // Header
};

MEM_Controller mem_default_controller = &st_default_controller;


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

static void set_header(Header* header, size_t size, char* filename, int line) {
    header->s.size = size;
    header->s.filename = filename;
    header->s.line = line;
    header->s.next = header->s.prev = NULL;
    memset(header->s.mark, MARK, (char*)&header[1] - (char*)header->s.mark);    
}

static void chain_header(MEM_Controller controller, Header* new_header) {
    if (controller->block_header == NULL) {
        printf("chain first\n");
        controller->block_header = new_header;
        return;
    }    
}

void MEM_dump_memory_func(MEM_Controller controller) {
    Header *current_header = NULL;
    if ((current_header = controller->block_header) == NULL) {
        printf("no allocated memory\n");
        return;
    }
    int i;
    uint8_t *ptr;
    while(current_header) {
        uint32_t alloc_size = current_header->s.size + sizeof(Header);
        printf("-----------------------------------\n");
        printf("-- size:%d, file:%s, line:%d -- \n", current_header->s.size, current_header->s.filename, current_header->s.line);
        printf("-----------------------------------\n");
        ptr = (uint8_t*)current_header;
        for (i = 0; i < alloc_size; ++i, ++ptr) {
            if (i % 16 == 0) printf("\n");
            printf("%02x ", *ptr);
        }
        printf("\n");                
        current_header = current_header->s.next;
    }
    

}


void MEM_malloc_func(MEM_Controller controller, char* filename, int line, size_t size) {
    uint8_t *ptr;
    uint32_t i;
    printf("call mem_malloc_func\n");
    uint32_t hsize = sizeof(Header);
    
    uint32_t alloc_size = sizeof(Header) + size;
    
    Header *header = (Header*)malloc(alloc_size);
    if (header == NULL) {
        printf("error");
        exit(1);
    }    
    
    memset((void*)header, 0xcc, alloc_size);
    
    /*
    ptr = (uint8_t*)header;
    for (i = 0; i < alloc_size; ++i, ++ptr) {
        if (i % 16 == 0) printf("\n");        
        printf("%02x ", *ptr);

    }
    printf("\n");
     */
    set_header(header, size, filename, line);
    chain_header(controller, header);
    
    /*
    ptr = (uint8_t*)header;
    for (i = 0; i < alloc_size; ++i, ++ptr) {
        if (i % 16 == 0) printf("\n");        
        printf("%02x ", *ptr);

    }
    printf("\n");
    */
    
}

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