#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mas.h"


void push_value(MAS_Interpreter* interp, MAS_Value* value) {
    if (interp->stack.stack_alloc_size - 1 == interp->stack.stack_pointer) {
        interp->stack.stack_alloc_size += STACK_ALLOC_SIZE;
        interp->stack.stack = (MAS_Value*)MEM_realloc(interp->stack.stack, 
                sizeof(MAS_Value) * interp->stack.stack_alloc_size);
        fprintf(stderr, "stack extend realloc\n");
    }
    interp->stack.stack[++interp->stack.stack_pointer] = *value;    
}

MAS_Value pop_value(MAS_Interpreter* interp) {
    if (interp->stack.stack_pointer == 0) {
        fprintf(stderr, "stack is empty\n");
        exit(1);
    }
    return interp->stack.stack[interp->stack.stack_pointer--];
}

MAS_Value* peek_stack(MAS_Interpreter* interp, int index) {
    int pos;
    if ((pos =interp->stack.stack_pointer - index) < 0) {
        fprintf(stderr, "no such value on stack\n");
        return NULL;
    }
    return &interp->stack.stack[pos];
}

void shrink_stack(MAS_Interpreter* interp, int s_size) {
    interp->stack.stack_pointer -= s_size;
}

static LocalEnvironment* create_environment(MAS_Interpreter* interp) {
    LocalEnvironment* new_env = (LocalEnvironment*)MEM_malloc(sizeof(LocalEnvironment));
    new_env->variable = NULL;
    new_env->global_variable = NULL;    
    new_env->next = interp->top;
    interp->top = new_env;
    return new_env;
}

static void dispose_environment(MAS_Interpreter* interp, LocalEnvironment* env) {
    Variable *pos, *next;
    interp->top = env->next;
    pos = env->variable;
    while(pos) {
        next = pos->next;
        MEM_free(pos);
        pos = next;
    }
    MEM_free(env);    
}


static void mas_eval_string_expression(MAS_Interpreter* interp, LocalEnvironment* env, 
        Expression* expr) {  
    MAS_Value val;
    val.type = MAS_STRING_VALUE;
    val.u.object_value = mas_literal_to_mas_ostring(interp, expr->u.string_value);
    push_value(interp, &val);
    
}

static void call_native_function(MAS_Interpreter* interp, LocalEnvironment* env,
        Expression* expr, MAS_NativeFunctionProc proc) {
    MAS_Value v;
    ArgumentList* arg_list = NULL;    
    int arg_count, i;
    for (arg_count = 0, arg_list = expr->u.function_call_expression.argument; arg_list;
            arg_list = arg_list->next, ++arg_count) {
        mas_eval_expression(interp, env, arg_list->expression);
    }

    MAS_Value* args = peek_stack(interp, arg_count-1);
    v = proc(interp, arg_count, args);
    shrink_stack(interp, arg_count);
    push_value(interp, &v);        
}

static void call_mas_function(MAS_Interpreter* interp, LocalEnvironment* env,
        Expression* expr, ParameterList* params, Block* block) {
//    fprintf(stderr, "---  call_mas_function  ---\n");
    MAS_Value v;
    ParameterList* p_pos;
    ArgumentList*  a_pos;
    int arg_count, par_count, i;
    for (par_count = 0, p_pos = params; p_pos; p_pos = p_pos->next, ++par_count);
    for (arg_count = 0, a_pos = expr->u.function_call_expression.argument; 
            a_pos; a_pos = a_pos->next, ++arg_count);

    if (par_count < arg_count) {
        mas_runtime_error(expr->line_number,
                ARGUMENT_TOO_MANY_ERR,
                MESSAGE_ARGUMENT_END);
    } else if (par_count > arg_count) {
        mas_runtime_error(expr->line_number,
                ARGUMENT_TOO_FEW_ERR,
                MESSAGE_ARGUMENT_END);
    }
    
    LocalEnvironment* new_env = create_environment(interp);
    Variable* var_p;
    for (i = 0, p_pos = params, a_pos = expr->u.function_call_expression.argument;
            i < arg_count; ++i, p_pos = p_pos->next, a_pos = a_pos->next) {
        var_p = mas_add_local_variable(new_env, p_pos->name);
        mas_eval_expression(interp, env, a_pos->expression);
        var_p->value = pop_value(interp);
    }
//    mas_run_gc(interp); // for test
//    mas_show_all_local_variable(new_env);
    StatementResult r = mas_execute_statementlist(interp, new_env, block->stmt_list);
    dispose_environment(interp, new_env);
    
    if (r.type == RETURN_STATEMENT_RESULT) {
        v = r.u.return_value;
    } else {
        v.type = MAS_NULL_VALUE;
    }
    
    push_value(interp, &v);
}

static void mas_eval_function_call_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    FunctionDefinition* func = mas_search_function(expr->u.function_call_expression.identifier);
    if (func == NULL) {
        mas_runtime_error(expr->line_number,
                FUNCTION_NOT_FOUND_ERR,
                STRING_MESSAGE_ARGUMENT, "name", expr->u.function_call_expression.identifier,
                MESSAGE_ARGUMENT_END);
    }
    
    switch(func->type) {
        case NATIVE_FUNCTION: {
            call_native_function(interp, env, expr, func->u.native_f.n_func);
            break;
        }
        case MAS_FUNCTION: {
            call_mas_function(interp, env, expr, func->u.mas_f.param, func->u.mas_f.block);
            break;
        }
        default: {
            fprintf(stderr, "no suhc func type %d in mas_eval_function_call_expression\n", func->type);
            exit(1);
            break;
        }
    }                         
}

static void mas_eval_int_expression(MAS_Interpreter* interp, 
        LocalEnvironment* env, Expression* expr) {
    MAS_Value v;
    v.type = MAS_INT_VALUE;
    v.u.int_value = expr->u.int_value;
    push_value(interp, &v);   
}

static void mas_eval_double_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value v;
    v.type = MAS_DOUBLE_VALUE;
    v.u.double_value = expr->u.double_value;
    push_value(interp, &v);
}


static void mas_eval_boolean_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value v;
    v.type = MAS_BOOLEAN_VALUE;
    v.u.boolean_value = expr->u.boolean_value;
    push_value(interp, &v);
}

static void mas_eval_null_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value v;
    v.type = MAS_NULL_VALUE;
    push_value(interp, &v);
}

static Variable* search_global_variable_from_env(MAS_Interpreter* interp, 
        LocalEnvironment* env, char* identifier) {
    if (env == NULL) {
        return MAS_search_global_variable(interp, identifier);
    }
    return NULL;
}
static MAS_Value* lookup_identifier_address(MAS_Interpreter* interp, 
        LocalEnvironment* env, char* identifier) {
    Variable* val = MAS_search_local_variable(env, identifier);
    if (val) { // find in local variable
        return &val->value;
    } else {
        val = search_global_variable_from_env(interp, env, identifier);
        if (val) {
            return &val->value;
        } else {                
            if (env) {
                val = mas_add_local_variable(env, identifier);
            } else {
                val = mas_add_global_variable(interp, identifier);
            }
        }
    }
    
    if (!val) {
        fprintf(stderr, "cannot allocate variable in lookup identifier address\n");
    }
    return &val->value;
}

static MAS_Value* lookup_index_address(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    Expression* arg_expr = expr->u.index_expression.array;
    Expression* i_expr   = expr->u.index_expression.index;
    mas_eval_expression(interp, env, arg_expr);
    mas_eval_expression(interp, env, i_expr);
    MAS_Value iv = pop_value(interp);
    MAS_Value av = pop_value(interp);

    if (av.type != MAS_ARRAY_VALUE) {
        mas_runtime_error(expr->line_number,
                        BAD_INDEX_VALUE_ERR,
                        MESSAGE_ARGUMENT_END);
    }
    if (iv.type != MAS_INT_VALUE) {
        mas_runtime_error(expr->line_number,
                        BAD_INDEX_TYPE_ERR,
                        MESSAGE_ARGUMENT_END);
    }    
    return &av.u.object_value->u.array.array[iv.u.int_value];
}

static MAS_Value* get_lvalue(MAS_Interpreter* interp, 
        LocalEnvironment* env, Expression* l_expr) {
    MAS_Value* l_valp = NULL;
    switch (l_expr->type) {
        case IDENTIFIER_EXPRESSION: {
            l_valp = lookup_identifier_address(interp, env, l_expr->u.identifier);
            break;
        }
        case INDEX_EXPRESSION: {
            l_valp = lookup_index_address(interp, env, l_expr);
            break;
        }
        default: {
            mas_runtime_error(l_expr->line_number,
                    BAD_LEFT_VALUE_ERR,
                    MESSAGE_ARGUMENT_END);
            break;
        }
    }
    
    return l_valp;
}

static void mas_eval_assignment_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
//    fprintf(stderr, "assignment expr\n");        
    MAS_Value* l_valp;    
    Expression* l_expr = expr->u.assign_expression.variable;
    Expression* r_expr = expr->u.assign_expression.operand;
    l_valp = get_lvalue(interp, env, l_expr);
    mas_eval_expression(interp, env, r_expr);
    *l_valp = *peek_stack(interp, 0);    

//    mas_show_all_global_variable(interp);    
}

static void mas_eval_identifier_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    char* identifier = expr->u.identifier;
    Variable* v = MAS_search_local_variable(env, identifier);
    if (v) {
        push_value(interp, &v->value);
    } else {
        v = search_global_variable_from_env(interp, env, identifier);
        if (v == NULL) {
            mas_runtime_error(expr->line_number,
                        VARIABLE_NOT_FOUND_ERR,
                        STRING_MESSAGE_ARGUMENT, "name", identifier,
                        MESSAGE_ARGUMENT_END);
        }
        push_value(interp, &v->value);
    }    
}

static void mas_eval_array_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value val;
    val.type = MAS_ARRAY_VALUE;
    ExpressionList* expr_list;
    int i, count;
    for (count = 0, expr_list = expr->u.array_literal; expr_list; expr_list = expr_list->next, ++count) {
        mas_eval_expression(interp, env, expr_list->expression);
    }
    
    val.u.object_value = mas_create_array_literal(interp, count);
    for (count -= 1; count >= 0; --count) {
        val.u.object_value->u.array.array[count] = pop_value(interp);
    }
    
    /*
    for (count = 0; count < 3; ++count) {
        fprintf(stderr, "value = %d\n", val.u.object_value->u.array.array[count].u.int_value);
    }
    */
    
    push_value(interp, &val);    
}

static void mas_eval_index_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {    
    MAS_Value v = *lookup_index_address(interp, env, expr);
    push_value(interp, &v);
}

static void mas_eval_incdec_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value* valp;
    MAS_Value val;
    valp = get_lvalue(interp, env, expr->u.inc_dec_expression.operand);
    if (valp->type != MAS_INT_VALUE) {
        fprintf(stderr, "inc/dec should be applied to int\n");
        exit(1);
    }
    if (expr->type == INCREMENT_EXPRESSION) {
        valp->u.int_value++;
    } else if (expr->type == DECREMENT_EXPRESSION) {
        valp->u.int_value--;
    }
    val = *valp;
    push_value(interp, &val);
    
}

static void binary_double(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr, double l, double r) {
    MAS_Value v;
    v.type = MAS_DOUBLE_VALUE;
    switch (expr->type) {
        case ADD_EXPRESSION: {
            v.u.double_value = l + r;
            goto RET_DOUBLE;
        }
        case SUB_EXPRESSION: {
            v.u.double_value = l - r;
            goto RET_DOUBLE;
        }
        case MUL_EXPRESSION: {
            v.u.double_value = l * r;
            goto RET_DOUBLE;
        }
        case DIV_EXPRESSION: {
            v.u.double_value = l / r;
            goto RET_DOUBLE;                        
        }
        case MOD_EXPRESSION: {
            v.u.double_value = fmod(l, r);
            goto RET_DOUBLE;
        }
        case EQ_EXPRESSION: {
            v.u.boolean_value = (l == r) ? MAS_TRUE : MAS_FALSE;
            goto RET_BOOL;
        }
        case NE_EXPRESSION: {
            v.u.boolean_value = (l != r) ? MAS_TRUE : MAS_FALSE;
            goto RET_BOOL;
        }
        case GT_EXPRESSION: {
            v.u.boolean_value = (l > r) ? MAS_TRUE : MAS_FALSE;                    
            goto RET_BOOL;
        }
        case GE_EXPRESSION: {
            v.u.boolean_value = (l >= r) ? MAS_TRUE : MAS_FALSE;
            goto RET_BOOL;
        }
        case LT_EXPRESSION: {
            v.u.boolean_value = (l < r) ? MAS_TRUE : MAS_FALSE;
            goto RET_BOOL;
        }
        case LE_EXPRESSION: {
            v.u.boolean_value = (l <= r) ? MAS_TRUE : MAS_FALSE;
            goto RET_BOOL;
        }

        default: {
            mas_runtime_error(expr->line_number,
                    BAD_OPERAND_TYPE_ERR,
                    STRING_MESSAGE_ARGUMENT, "operator", mas_get_operator_string(expr->type),
                    MESSAGE_ARGUMENT_END);
        }
    }
    
    RET_BOOL:
            v.type = MAS_BOOLEAN_VALUE;
    RET_DOUBLE:  
            
    pop_value(interp);
    pop_value(interp);
    push_value(interp, &v);    
}

static void binary_int(MAS_Interpreter* interp, 
        LocalEnvironment* env, Expression* expr, int l, int r) {
    MAS_Value v;
    v.type = MAS_INT_VALUE;    
    switch (expr->type) {
        case ADD_EXPRESSION: {
            v.u.int_value = l + r;
            goto RET_INT;
        }
        case SUB_EXPRESSION: {
            v.u.int_value = l - r;
            goto RET_INT;
        }
        case MUL_EXPRESSION: {
            v.u.int_value = l * r;
            goto RET_INT;
        }        
        case DIV_EXPRESSION: {
            v.u.int_value = l / r;
            goto RET_INT;
        }
        case MOD_EXPRESSION: {
            v.u.int_value = l % r;
            goto RET_INT;
        }
        case EQ_EXPRESSION: {
            v.u.boolean_value = (l == r) ? MAS_TRUE : MAS_FALSE;
            goto RET_BOOL;
        }
        case NE_EXPRESSION: {
            v.u.boolean_value = (l != r) ? MAS_TRUE : MAS_FALSE;
            goto RET_BOOL;
        }
        case GT_EXPRESSION: {
            v.u.boolean_value = (l > r) ? MAS_TRUE : MAS_FALSE;                    
            goto RET_BOOL;
        }
        case GE_EXPRESSION: {
            v.u.boolean_value = (l >= r) ? MAS_TRUE : MAS_FALSE;
            goto RET_BOOL;
        }
        case LT_EXPRESSION: {
            v.u.boolean_value = (l < r) ? MAS_TRUE : MAS_FALSE;
            goto RET_BOOL;
        }
        case LE_EXPRESSION: {
            v.u.boolean_value = (l <= r) ? MAS_TRUE : MAS_FALSE;
            goto RET_BOOL;
        }
        default: {
            mas_runtime_error(expr->line_number,
                    BAD_OPERAND_TYPE_ERR,
                    STRING_MESSAGE_ARGUMENT, "operator", mas_get_operator_string(expr->type),
                    MESSAGE_ARGUMENT_END);
        }
    }
    
    RET_BOOL:
            v.type = MAS_BOOLEAN_VALUE;

    RET_INT:    
    pop_value(interp);
    pop_value(interp);
    push_value(interp, &v);    
}

static void mas_binary_string(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr, MAS_Object* l_str, MAS_Object* r_str) {
    MAS_Value v;    
    switch (expr->type) {
        case ADD_EXPRESSION: {
            int n_len, l_len, r_len;
            char* buf;
            n_len = (l_len = strlen(l_str->u.string.string)) + (r_len = strlen(r_str->u.string.string));
            buf = (char*)MEM_malloc(n_len + 1);
            strncpy(buf, l_str->u.string.string, l_len);
            strncpy(&buf[l_len], r_str->u.string.string, r_len+1);
            MAS_Object* n_obj = mas_create_mas_ostring(interp, buf);
            v.type = MAS_STRING_VALUE;
            v.u.object_value = n_obj;
            break;
        }
        case EQ_EXPRESSION: {
            v.type = MAS_BOOLEAN_VALUE;
            v.u.boolean_value = 
                    (strcmp(l_str->u.string.string, r_str->u.string.string) == 0) ? MAS_TRUE : MAS_FALSE;
            break;
        }
        case NE_EXPRESSION: {
            v.type = MAS_BOOLEAN_VALUE;
            v.u.boolean_value = 
                    (strcmp(l_str->u.string.string, r_str->u.string.string) != 0) ? MAS_TRUE : MAS_FALSE;
            break;
        }
        default: {
            break;
        }
    }    
    pop_value(interp);
    pop_value(interp);
    push_value(interp, &v);

//    fprintf(stderr, "----------------->>>>> run gc in add\n");
//    mas_run_gc(interp); // for testq
}

typedef enum {
    L_STR = 1,
    R_STR,
    S_DIR_PLUS_1            
} S_Dir;

static void mas_binary_string_int(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr, MAS_Object* str, int val, S_Dir d) {
    
    switch (expr->type) {
        case ADD_EXPRESSION: {
            char buf[1024];
            sprintf(buf, "%d", val);
            char* add_str = (char*)MEM_malloc(strlen(buf) + 1);
            strncpy(add_str, buf, strlen(buf) + 1);
            MAS_Object* add_obj = mas_create_mas_ostring(interp, add_str);
            switch (d) {
                case L_STR: {
                    mas_binary_string(interp, env, expr, str, add_obj);
                    return;
                }
                case R_STR: {
                    mas_binary_string(interp, env, expr, add_obj, str);
                    return;
                }
                default: {
                    fprintf(stderr, "No Such S_Dir\n");
                    exit(1);
                }
            }
            break;
        }
        default: {
            mas_runtime_error(expr->line_number,
                    BAD_OPERAND_TYPE_ERR,
                    STRING_MESSAGE_ARGUMENT, "operator", mas_get_operator_string(expr->type),
                    MESSAGE_ARGUMENT_END);
        }
    }
}

static void mas_binary_string_double(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr, MAS_Object* str, double val, S_Dir d) {    
    switch (expr->type) {
        case ADD_EXPRESSION: {
            char buf[1024];
            sprintf(buf, "%f", val);
            char* add_str = (char*)MEM_malloc(strlen(buf) + 1);
            strncpy(add_str, buf, strlen(buf) + 1);
            MAS_Object* add_obj = mas_create_mas_ostring(interp, add_str);
            switch (d) {
                case L_STR: {
                    mas_binary_string(interp, env, expr, str, add_obj);
                    return;
                }
                case R_STR: {
                    mas_binary_string(interp, env, expr, add_obj, str);
                    return;
                }
                default: {
                    fprintf(stderr, "No Such S_Dir\n");
                    exit(1);
                }
            }
            break;
        }
        default: {
            mas_runtime_error(expr->line_number,
                    BAD_OPERAND_TYPE_ERR,
                    STRING_MESSAGE_ARGUMENT, "operator", mas_get_operator_string(expr->type),
                    MESSAGE_ARGUMENT_END);
        }
    }
}

static void mas_binary_string_boolean(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr, MAS_Object* str, MAS_Boolean val, S_Dir d) {  
    switch(expr->type) {
        case ADD_EXPRESSION: {
             char buf[1024];
            (val == MAS_FALSE) ?  
                strncpy(buf, "false", 6) : strncpy(buf, "true", 5);
            char* add_str = (char*)MEM_malloc(strlen(buf) + 1);
            strncpy(add_str, buf, strlen(buf) + 1);
            MAS_Object* add_obj = mas_create_mas_ostring(interp, add_str);
            switch (d) {
                case L_STR: {
                    mas_binary_string(interp, env, expr, str, add_obj);
                    return;
                }
                case R_STR: {
                    mas_binary_string(interp, env, expr, add_obj, str);
                    return;
                }
                default: {
                    fprintf(stderr, "No Such S_Dir\n");
                    exit(1);
                }
            }
            break;
            
        }
        default: {
            mas_runtime_error(expr->line_number,
                    BAD_OPERAND_TYPE_ERR,
                    STRING_MESSAGE_ARGUMENT, "operator", mas_get_operator_string(expr->type),
                    MESSAGE_ARGUMENT_END);
        }
    }
    
}

static void mas_binary_string_null(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr, MAS_Object* str, S_Dir d) {
    MAS_Value v;
    v.type = MAS_BOOLEAN_VALUE;
    switch (expr->type) {
        case ADD_EXPRESSION: {
            char buf[1024];
            strncpy(buf, "null", 5);
            char* add_str = (char*)MEM_malloc(strlen(buf) + 1);
            strncpy(add_str, buf, strlen(buf) + 1);
            MAS_Object* add_obj = mas_create_mas_ostring(interp, add_str);
            switch (d) {
                case L_STR: {
                    mas_binary_string(interp, env, expr, str, add_obj);
                    return;
                }
                case R_STR: {
                    mas_binary_string(interp, env, expr, add_obj, str);
                    return;
                }
                default: {
                    fprintf(stderr, "No Such S_Dir\n");
                    exit(1);
                }
            }
        }
        case EQ_EXPRESSION: {
            v.u.boolean_value = MAS_FALSE;
            break;
        }
        case NE_EXPRESSION: {
            v.u.boolean_value = MAS_TRUE;
            break;
        }
        default: {
            mas_runtime_error(expr->line_number,
                    BAD_OPERAND_TYPE_ERR,
                    STRING_MESSAGE_ARGUMENT, "operator", mas_get_operator_string(expr->type),
                    MESSAGE_ARGUMENT_END);
        }
    }
    pop_value(interp);
    pop_value(interp);
    push_value(interp, &v);
}


static void mas_binary_expression(MAS_Interpreter* interp, 
        LocalEnvironment* env, Expression* expr) {
    mas_eval_expression(interp, env, expr->u.binary_expression.left);
    mas_eval_expression(interp, env, expr->u.binary_expression.right);
    MAS_Value* l_valp = peek_stack(interp, 1);
    MAS_Value* r_valp = peek_stack(interp, 0);
    
    if (l_valp->type == MAS_INT_VALUE && r_valp->type == MAS_INT_VALUE) { // int int
        binary_int(interp, env, expr, l_valp->u.int_value, r_valp->u.int_value);
    } else if (l_valp->type == MAS_INT_VALUE && r_valp->type == MAS_DOUBLE_VALUE) { // int double
        binary_double(interp, env, expr, l_valp->u.int_value, r_valp->u.double_value);
    } else if (l_valp->type == MAS_DOUBLE_VALUE && r_valp->type == MAS_INT_VALUE) { // double int
        binary_double(interp, env, expr, l_valp->u.double_value, r_valp->u.int_value);
    } else if (l_valp->type == MAS_DOUBLE_VALUE && r_valp->type == MAS_DOUBLE_VALUE) { // double double
        binary_double(interp, env, expr, l_valp->u.double_value, r_valp->u.double_value);
    } else if (l_valp->type == MAS_STRING_VALUE && r_valp->type == MAS_STRING_VALUE) { // string string
        mas_binary_string(interp, env, expr, l_valp->u.object_value, r_valp->u.object_value);
    } else if (l_valp->type == MAS_STRING_VALUE && r_valp->type == MAS_INT_VALUE) { // string int
        mas_binary_string_int(interp, env, expr, l_valp->u.object_value, r_valp->u.int_value, L_STR);
    } else if (l_valp->type == MAS_INT_VALUE && r_valp->type == MAS_STRING_VALUE) { // int string
        mas_binary_string_int(interp, env, expr, r_valp->u.object_value, l_valp->u.int_value, R_STR);
    } else if (l_valp->type == MAS_STRING_VALUE && r_valp->type == MAS_DOUBLE_VALUE) { // string double
        mas_binary_string_double(interp, env, expr, l_valp->u.object_value, r_valp->u.double_value, L_STR);
    } else if (l_valp->type == MAS_DOUBLE_VALUE && r_valp->type == MAS_STRING_VALUE) { // double string
        mas_binary_string_double(interp, env, expr, r_valp->u.object_value, l_valp->u.double_value, R_STR);
    } else if (l_valp->type == MAS_STRING_VALUE && r_valp->type == MAS_BOOLEAN_VALUE) { // string boolean
        mas_binary_string_boolean(interp, env, expr, l_valp->u.object_value, r_valp->u.boolean_value, L_STR);
    } else if (l_valp->type == MAS_BOOLEAN_VALUE && r_valp->type == MAS_STRING_VALUE) { // boolean string
        mas_binary_string_boolean(interp, env, expr, r_valp->u.object_value, l_valp->u.boolean_value, R_STR);
    } else if (l_valp->type == MAS_STRING_VALUE && r_valp->type == MAS_NULL_VALUE) {
        mas_binary_string_null(interp, env, expr, l_valp->u.object_value, L_STR);
    } else if (l_valp->type == MAS_NULL_VALUE && r_valp->type == MAS_STRING_VALUE) {
        mas_binary_string_null(interp, env, expr, r_valp->u.object_value, R_STR);
    } else {
        mas_runtime_error(expr->line_number,
                BAD_OPERAND_TYPE_ERR,
                STRING_MESSAGE_ARGUMENT, "operator", mas_get_operator_string(expr->type),
                MESSAGE_ARGUMENT_END);
    }
}


static void mas_eval_logical_and_or_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value v;
    v.type = MAS_BOOLEAN_VALUE;
    mas_eval_expression(interp, env, expr->u.binary_expression.left);
    mas_eval_expression(interp, env, expr->u.binary_expression.right);    
    MAS_Value* l_valp = peek_stack(interp, 1);
    MAS_Value* r_valp = peek_stack(interp, 0);
    

    if (l_valp->type == MAS_BOOLEAN_VALUE && r_valp->type == MAS_BOOLEAN_VALUE) {
        switch (expr->type) {
            case LOGICAL_AND_EXPRESSION: {
                v.u.boolean_value = (l_valp->u.boolean_value && r_valp->u.boolean_value);
                break;
            }
            case LOGICAL_OR_EXPRESSION: {
                v.u.boolean_value = (l_valp->u.boolean_value || r_valp->u.boolean_value);
                break;
            }
            default: {
                mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", mas_get_operator_string(expr->type),
                        MESSAGE_ARGUMENT_END);
                break;
            }
        }        
    } else {
        mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", mas_get_operator_string(expr->type),
                        MESSAGE_ARGUMENT_END);
    }    
    pop_value(interp);
    pop_value(interp);
    push_value(interp, &v);    
}

static void mas_eval_minus_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value v;

    mas_eval_expression(interp, env, expr->u.minus_expression);
    MAS_Value* valp = peek_stack(interp, 0);
    
    if (valp->type != MAS_INT_VALUE &&
            valp->type != MAS_DOUBLE_VALUE) {
        mas_runtime_error(expr->line_number,
                MINUS_OPERAND_TYPE_ERR,
                MESSAGE_ARGUMENT_END);
    }

    switch (valp->type) {
        case MAS_INT_VALUE: {
            v.u.int_value = - valp->u.int_value;
            v.type = MAS_INT_VALUE;
            break;
        }
        case MAS_DOUBLE_VALUE: {
            v.u.double_value = - valp->u.double_value;
            v.type = MAS_DOUBLE_VALUE;
            break;
        }
        default: {
            exit(1);
        }        
    }
    pop_value(interp);
    push_value(interp, &v);
}

void mas_eval_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    

    switch (expr->type) {
        case BOOLEAN_EXPRESSION: {
            mas_eval_boolean_expression(interp, env, expr);
            break;
        }
        case INT_EXPRESSION: {            
            mas_eval_int_expression(interp, env, expr);
            break;
        }
        case DOUBLE_EXPRESSION: {
            mas_eval_double_expression(interp, env, expr);
            break;
        }
        case NULL_EXPRESSION: {
            mas_eval_null_expression(interp, env, expr);
            break;
        }
        case STRING_EXPRESSION: {
            mas_eval_string_expression(interp, env, expr);
            break;
        }
        case IDENTIFIER_EXPRESSION: {
            mas_eval_identifier_expression(interp, env, expr);
            break;
        }
        case ASSIGN_EXPRESSION: {
            mas_eval_assignment_expression(interp, env, expr);
            break;
        }
        case FUNCTION_CALL_EXPRESSION: {
            mas_eval_function_call_expression(interp, env, expr);
            break;
        }
        case ARRAY_EXPRESSION: {
            mas_eval_array_expression(interp, env, expr);
            break;
        }
        case INDEX_EXPRESSION: {
            mas_eval_index_expression(interp, env, expr);
            break;
        }
        case INCREMENT_EXPRESSION: 
        case DECREMENT_EXPRESSION: {
            mas_eval_incdec_expression(interp, env, expr);
            break;
        }
        case ADD_EXPRESSION: 
        case SUB_EXPRESSION: 
        case MUL_EXPRESSION: 
        case DIV_EXPRESSION: 
        case MOD_EXPRESSION:
        case EQ_EXPRESSION: 
        case NE_EXPRESSION: 
        case GT_EXPRESSION: 
        case GE_EXPRESSION:
        case LT_EXPRESSION: 
        case LE_EXPRESSION: {
            mas_binary_expression(interp, env, expr);
            break;
        }
        case LOGICAL_OR_EXPRESSION:
        case LOGICAL_AND_EXPRESSION: {
            mas_eval_logical_and_or_expression(interp, env, expr);
            break;
        }
        case MINUS_EXPRESSION: {
            mas_eval_minus_expression(interp, env, expr);
            break;
        }
        default: {
            fprintf(stderr, "undefined expression type in mas_eval_expression %d\n", expr->type);
            exit(1);
        }
    }      
}
