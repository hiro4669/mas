
#include "info.h"
//#include "./memory/MEM.h"

static LocalInfo *l_info;

void mas_init_localinfo() {
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
    MEM_free(l_info);
}

void* mas_malloc(MEM_Storage storage, size_t size) {
    return MEM_storage_malloc(storage, size);
}