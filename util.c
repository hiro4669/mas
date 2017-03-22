
#include <string.h>
#include "info.h"
//#include "./memory/MEM.h"

static LocalInfo *l_info = NULL;

void mas_init_localinfo() {
    if (l_info) return;
    l_info = (LocalInfo*)MEM_malloc(sizeof(LocalInfo));
    l_info->line_number = 1;
}
void mas_set_localinfo(LocalInfo *info) {
    l_info = info;
}
LocalInfo* mas_get_localinfo() {
    return l_info;
}
void mas_delete_localinfo() {
    if (l_info) MEM_free(l_info);
}

void* mas_malloc(MEM_Storage storage, size_t size) {
    return MEM_storage_malloc(storage, size);
}

FunctionDefinition* mas_search_function(const char* name) {
    FunctionDefinition* pos = NULL;
    MAS_Interpreter* interp;
    interp = mas_get_interpreter();
    for (pos = interp->func_list; pos; pos = pos->next) {
        if (!strcmp(name, pos->name)) break;           
    }
    return pos;
}
