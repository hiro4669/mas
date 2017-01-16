#include <stdio.h>
#include "memory.h"

typedef union {
    long   l_dummy;
    double d_dummy;
    void   *p_dummy;
} Cell;


#define CELL_SIZE            (sizeof(Cell))
#define DEFAULT_PAGE_SIZE    (1024)

typedef struct MemoryPage_tag MemoryPage;
typedef MemoryPage *MemoryPageList;


struct MemoryPage_tag {
    int            cell_num;
    int            use_cell_num;
    MemoryPageList next;
    Cell           cell[1];
};

struct MEM_Storage_tag {
    MemoryPageList page_list;
    int            current_page_size;  // number of pages
};

MEM_Storage MEM_open_storage_func(MEM_Controller controller,
        char* filename, int line, int page_size) {    
    MEM_Storage storage;
    storage = (MEM_Storage)MEM_malloc_func(controller, filename, line, sizeof(struct MEM_Storage_tag));
    storage->page_list = NULL;
    storage->current_page_size = page_size;
    if (page_size == 0) {
        storage->current_page_size = DEFAULT_PAGE_SIZE;
    }
    return storage;
}

void* MEM_storage_malloc_func(MEM_Controller controller, char* filename, int line, MEM_Storage storage, size_t size) {
    
    if (storage->page_list == NULL) { /* do not keep enough memory*/
        int cellnum = ((size - 1) / CELL_SIZE) + 1;
        printf("cellnum = %d\n", cellnum);
        int alloc_num = cellnum > storage->current_page_size ? cellnum : storage->current_page_size;
        MemoryPageList page_list = (MemoryPageList)MEM_malloc_func(controller, filename, line, sizeof(MemoryPage) 
                + (alloc_num - 1) * CELL_SIZE);
        printf("MemoryPage size: %d, alloc_num: %d, cellsize: %d\n", (int)sizeof(MemoryPage), (int)alloc_num, (int)CELL_SIZE);
        printf("storage_malloc_size = %d\n", (int)(sizeof(MemoryPage) + (alloc_num-1) * CELL_SIZE));
        page_list->cell_num = alloc_num;
        page_list->use_cell_num = cellnum;
        page_list->next = NULL;
        storage->page_list = page_list;
        return &page_list->cell[0];
        
    } else {
        
    }

    
    
    return NULL;
}

