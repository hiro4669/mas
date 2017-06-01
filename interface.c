#include <stdio.h>
#include <stdlib.h>

#include "mas.h"
#include "info.h"


static MAS_Interpreter *mas_interpreter;

static void add_native_functions() {
    MAS_add_native_function("print", mas_nv_print);
    MAS_add_native_function("fopen", mas_nv_open);
    MAS_add_native_function("fclose", mas_nv_close);
    MAS_add_native_function("fputs", mas_nv_fputs);
    MAS_add_native_function("fgets", mas_nv_fgets);
}

static void release_global_variables(MAS_Interpreter* interp) {
    Variable* val;
    val = interp->variable;
    while(val) {
        if (val->value.type == MAS_STRING_VALUE) {
            mas_release_string(val->value.u.string_value);
        }
        val = val->next;        
    }    
}

MAS_Interpreter* mas_create_interpreter() {
    
    mas_init_localinfo();
    MEM_Storage ast_storage = MEM_open_storage(0);
    mas_interpreter = MEM_storage_malloc(ast_storage, sizeof(struct MAS_Interpreter_tag));
    mas_interpreter->ast_storage = ast_storage;
    mas_interpreter->execution_storage = NULL;
    mas_interpreter->variable = NULL;
    mas_interpreter->stmt = NULL;
    mas_interpreter->stmt_list = NULL;
    mas_interpreter->func_list = NULL;
    mas_interpreter->stack.stack = (MAS_Value*)MEM_malloc(sizeof(MAS_Value) * STACK_ALLOC_SIZE);
    mas_interpreter->stack.stack_alloc_size = STACK_ALLOC_SIZE;
    mas_interpreter->stack.stack_pointer = 0;
    mas_interpreter->heap.current_heap_size = 0;
    mas_interpreter->heap.current_threshold = 30; // should be changed
    mas_interpreter->heap.header = NULL;

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
    
    release_global_variables(mas_interpreter);
    
    if (mas_interpreter->execution_storage) {
        MEM_dispose(mas_interpreter->execution_storage);
    }    
    MEM_free(mas_interpreter->stack.stack);
    MEM_dispose(mas_interpreter->ast_storage);

}

void MAS_compile(MAS_Interpreter* interp, FILE* fp) {
    extern FILE *yyin;
    extern int yyparse(void);
    
    yyin = fp;
    if (yyparse()) {
        fprintf(stderr, "Error Error Error :line %d\n", mas_get_localinfo()->line_number);
        exit(1);
    }
    mas_reset_string_literal();
}

void MAS_interpret(MAS_Interpreter* interp) {
    interp->execution_storage = MEM_open_storage(0);
    mas_execute_statementlist(interp, NULL, interp->stmt_list);    
}

void mas_traverse_test() {
    MAS_Interpreter* interp = mas_get_interpreter();
    Visitor* visitor = create_visitor();
    
    fprintf(stderr, "-- traverse statement ---\n");    
    StatementList *slist;
    if (interp->stmt_list) {
        for (slist = interp->stmt_list; slist; slist = slist->next) {
            traverse_stmt(slist->statement, visitor);
        }
    }
    fprintf(stderr, "-- traverse function ---\n");    
    FunctionDefinition* flist; 
    if (interp->func_list) { // traverse function list
        for (flist = interp->func_list; flist; flist = flist->next) {
            traverse_func(flist, visitor);
        }
    }

}

void MAS_add_native_function(char* name, MAS_NativeFunctionProc proc) {
    MAS_Interpreter* interp = mas_get_interpreter();
    FunctionDefinition* func = (FunctionDefinition*)mas_malloc(interp->ast_storage, sizeof(FunctionDefinition));
    func->type = NATIVE_FUNCTION;
    func->name = name;
    func->next = NULL;
    func->u.native_f.n_func = proc;
    interp->func_list = mas_chain_function_definition(interp->func_list, func);    
}