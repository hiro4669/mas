#ifndef _MEM_H_
#define _MEM_H_

typedef struct MEM_Controller_tag *MEM_Controller;

extern MEM_Controller mem_default_controller;

#define MEM_malloc(size) MEM_malloc_func(mem_default_controller, __FILE__, __LINE__, size)
#define MEM_dump_memory() MEM_dump_memory_func(mem_default_controller)
#define MEM_free(ptr) MEM_free_func(mem_default_controller, ptr)

void test();
void *MEM_malloc_func(MEM_Controller controller, char* filename, int line, size_t size);
void MEM_dump_memory_func(MEM_Controller controller);
void MEM_free_func(MEM_Controller controller, void* bptr);

#endif
