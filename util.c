
#include <string.h>
#include <stdlib.h>
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


static void show_value(Variable* valp) {
    switch (valp->value.type) {
        case MAS_BOOLEAN_VALUE: {
                printf("val(boolean) = %s(%d)\n", valp->name, valp->value.u.boolean_value);
                break;
            }
            case MAS_INT_VALUE: {
                printf("val(int)     = %s(%d)\n", valp->name, valp->value.u.int_value);
                break;
            }
            case MAS_DOUBLE_VALUE: {
                printf("val(double)  = %s(%f)\n", valp->name, valp->value.u.double_value);                
                break;
            }
            case MAS_STRING_VALUE: {
                printf("val(string)  = %s(%p)\n", valp->name, valp->value.u.object_value->u.string.string);
                break;
            }
            case MAS_NATIVE_POINTER_VALUE: {
                printf("val(pointer) = %s(%p)\n", valp->name, valp->value.u.native_pointer.pointer);
                break;
            }
            case MAS_NULL_VALUE: {
                printf("val(null) = %s(null)\n", valp->name);
                break;
            }
            case MAS_ARRAY_VALUE: {
                printf("val(array) = %s\n", valp->name);
                break;
            }
            default: {
                fprintf(stderr, "no such mas type\n");                
                break;
            }       
    }
}

void mas_show_all_local_variable(LocalEnvironment* env) {
    printf("--- show all global variable ---\n");
    Variable* pos = NULL;
    for (pos = env->variable; pos; pos = pos->next) {
        show_value(pos);
    }
    
    
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
                printf("val(string)  = %s(%p)\n", pos->name, pos->value.u.object_value->u.string.string);
//                printf("val(string)  = %s(%p)\n", pos->name, pos->value.u.string_value->string);                
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
            case MAS_ARRAY_VALUE: {
                printf("val(array) = %s\n", pos->name);
                break;
            }
            default: {
                fprintf(stderr, "no such mas type\n");                
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

Variable* mas_add_local_variable(LocalEnvironment* env, char* identifier) {
    Variable *nv = (Variable*)MEM_malloc(sizeof(Variable));
    nv->name = identifier;
    nv->next = env->variable;
    env->variable = nv;
    nv->value.type = MAS_NULL_VALUE;    
    return nv;
}

Variable* mas_add_global_variable(MAS_Interpreter* interp, char* identifier) {
    Variable* nv = (Variable*)execute_malloc(sizeof(Variable));
    nv->name = (char*)execute_malloc(strlen(identifier) + 1);
    strcpy(nv->name, identifier);
    nv->next = interp->variable;
    interp->variable = nv;
    nv->value.type = MAS_NULL_VALUE;
    return nv;
    
}

char* mas_get_operator_string(ExpressionType type) {
    switch(type) {
        case ASSIGN_EXPRESSION:      return "=";
        case ADD_EXPRESSION:         return "+";
        case SUB_EXPRESSION:         return "-";
        case MUL_EXPRESSION:         return "*";
        case DIV_EXPRESSION:         return "/";
        case MOD_EXPRESSION:         return "%";
        case EQ_EXPRESSION:          return "==";
        case NE_EXPRESSION:          return "!=";
        case GT_EXPRESSION:          return ">";
        case GE_EXPRESSION:          return "=>";
        case LT_EXPRESSION:          return "<";
        case LE_EXPRESSION:          return "<=";
        case LOGICAL_AND_EXPRESSION: return "&&";
        case LOGICAL_OR_EXPRESSION:  return "||";
        default: {
            fprintf(stderr, "bad expression for operator %d\n", type);
            exit(1);
        }
    }
}
