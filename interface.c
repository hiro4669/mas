#include <stdio.h>
#include "mas.h"


static MAS_Interpreter *mas_interpreter;

static void add_native_functions() {
    MAS_add_native_function("print", mas_nv_print);
}

MAS_Interpreter* mas_create_interpreter() {
    
    mas_init_localinfo();
    MEM_Storage ast_storage = MEM_open_storage(0);
    mas_interpreter = MEM_storage_malloc(ast_storage, sizeof(struct MAS_Interpreter_tag));
    mas_interpreter->ast_storage = ast_storage;
    mas_interpreter->stmt = NULL;
    mas_interpreter->stmt_list = NULL;
    mas_interpreter->func_list = NULL;
    
    add_native_functions();
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

void MAS_add_native_function(char* name, MAS_NativeFunctionProc* proc) {
    MAS_Interpreter* interp = mas_get_interpreter();
    FunctionDefinition* func = (FunctionDefinition*)mas_malloc(interp->ast_storage, sizeof(FunctionDefinition));
    func->type = NATIVE_FUNCTION;
    func->name = name;
    func->next = NULL;
    func->u.native_f.n_func = proc;
    interp->func_list = mas_chain_function_definition(interp->func_list, func);    
}