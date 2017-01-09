#ifndef _MEM_H_
#define _MEM_H_


typedef struct MEM_Controller_tag *MEM_Controller;
typedef struct MEM_Storage_tag    *MEM_Storage;

extern MEM_Controller mem_default_controller;

/* Malloc */
#define MEM_malloc(size) MEM_malloc_func(mem_default_controller, __FILE__, __LINE__, size)
#define MEM_dump_memory() MEM_dump_memory_func(mem_default_controller)
#define MEM_free(ptr) MEM_free_func(mem_default_controller, ptr)

/* Storage */
#define MEM_open_strage(page_size) MEM_open_storage_func(mem_default_controller, __FILE__, __LINE__, page_size)

void test();
/* Malloc */
void *MEM_malloc_func(MEM_Controller controller, char* filename, int line, size_t size);
void MEM_dump_memory_func(MEM_Controller controller);
void MEM_free_func(MEM_Controller controller, void* bptr);

/* Storage */
MEM_Storage MEM_open_storage_func(MEM_Controller controller,
        char* filename, int line, int page_size);

#endif
