
#include "mas.h"


static MAS_Interpreter *mas_interpreter;


MAS_Interpreter* mas_create_interpreter() {
    
    mas_init_localinfo();
    MEM_Storage ast_storage = MEM_open_storage(0);
    mas_interpreter = MEM_storage_malloc(ast_storage, sizeof(struct MAS_Interpreter_tag));
    mas_interpreter->ast_storage = ast_storage;
    mas_interpreter->stmt = NULL;
    mas_interpreter->stmt_list = NULL;
    mas_interpreter->func_list = NULL;
    return mas_interpreter;
}

MAS_Interpreter* mas_get_interpreter() {
    if (mas_interpreter == NULL) mas_create_interpreter();
    return mas_interpreter;            
}

void mas_delete_interpreter() {
    mas_delete_localinfo();
    if (mas_interpreter == NULL) return;    
    MEM_dispose(mas_interpreter->ast_storage);
}
