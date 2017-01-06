#ifndef _MEM_H_
#define _MEM_H_

typedef struct MEM_Controller_tag *MEM_Controller;

extern MEM_Controller mem_default_controller;

#define MEM_malloc(size) MEM_malloc_func(mem_default_controller, __FILE__, __LINE__, size)

void test();
void MEM_malloc_func(MEM_Controller controller, char* filename, int line, size_t size);

#endif
