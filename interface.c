
#include "mas.h"


static MAS_Interpreter *mas_interpreter;


MAS_Interpreter* mas_create_interpreter() {
    MEM_Storage ast_storage = MEM_open_storage(0);
    mas_interpreter = MEM_storage_malloc(ast_storage, sizeof(struct MAS_Interpreter_tag));
    mas_interpreter->ast_storage = ast_storage;
    return mas_interpreter;
}

MAS_Interpreter* mas_get_interpreter() {
    if (mas_interpreter == NULL) mas_create_interpreter();
    return mas_interpreter;            
}

void mas_delete_interpreter() {
    if (mas_interpreter == NULL) return;    
    MEM_dispose(mas_interpreter->ast_storage);
}
