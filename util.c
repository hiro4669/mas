
#include "info.h"

static LocalInfo *l_info;

void mas_set_localinfo(LocalInfo *info) {
    l_info = info;
}

LocalInfo* mas_get_localinfo() {
    return l_info;
}
