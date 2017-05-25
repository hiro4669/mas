
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

static void* execute_malloc(size_t size) {
    MAS_Interpreter *interp = mas_get_interpreter();
    return mas_malloc(interp->execution_storage, size);
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

Variable* MAS_search_global_variable(MAS_Interpreter* interp, char* identifier) {
    Variable* pos = NULL;
    for (pos = interp->variable; pos; pos = pos->next) {
        if (!strcmp(identifier, pos->name)) { // find
            return pos;
        }
    }    
    return pos;
}

void mas_show_all_global_variable(MAS_Interpreter* interp) {
    printf("--- show all global variable ---\n");
    Variable* pos = NULL;
    for (pos = interp->variable; pos; pos = pos->next) {
        switch(pos->value.type) {
            case MAS_BOOLEAN_VALUE: {
                printf("val(boolean) = %s(%d)\n", pos->name, pos->value.u.boolean_value);
                break;
            }
            case MAS_INT_VALUE: {
                printf("val(int)     = %s(%d)\n", pos->name, pos->value.u.int_value);
                break;
            }
            case MAS_DOUBLE_VALUE: {
                printf("val(double)  = %s(%f)\n", pos->name, pos->value.u.double_value);                
                break;
            }
            case MAS_STRING_VALUE: {
                printf("val(string)  = %s(%p)\n", pos->name, pos->value.u.string_value->string);
                break;
            }
            case MAS_NATIVE_POINTER_VALUE: {
                printf("val(pointer) = %s(%p)\n", pos->name, pos->value.u.native_pointer.pointer);
                break;
            }
            case MAS_NULL_VALUE: {
                printf("val(null) = %s(null)\n", pos->name);
                break;
            }
            default: {
                fprintf(stderr, "no such mas type");                
                break;
            }       
        }

    }    
}

void MAS_add_global_variable(MAS_Interpreter* interp, char* identifier, MAS_Value *v) {
    Variable* nv = (Variable*)execute_malloc(sizeof(Variable));
    nv->name = (char*)execute_malloc(strlen(identifier) + 1); // identifierの領域はast_mallocで確保されているからこぴらなくてもいいのでは？
    strcpy(nv->name, identifier);
    nv->next = interp->variable;
    interp->variable = nv;
    nv->value = *v;
}

Variable* MAS_search_local_variable(LocalEnvironment* env, char* identifier) {
    Variable* pos;
    if (env == NULL) {
        return NULL;
    }
    for (pos = env->variable; pos; pos = pos->next) {
        if (!strcmp(identifier, pos->name)) {
            return pos;
        }
    }    
    return NULL;
}

void MAS_add_local_variable(LocalEnvironment* env, char* identifier, MAS_Value *v) {
    //Variable* nv = (Variable*)execute_malloc(sizeof(Variable));
    Variable* nv = (Variable*)MEM_malloc(sizeof(Variable));
    nv->name = identifier;
    nv->next = env->variable;
    env->variable = nv;
    nv->value = *v;    
}
