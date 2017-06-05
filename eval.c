#include <stdio.h>
#include <stdlib.h>
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


static void mas_eval_string_expression(MAS_Interpreter* interp, LocalEnvironment* env, 
        Expression* expr) {
    
    fprintf(stderr, "call eval_string expression\n");
    MAS_Value val;
    val.type = MAS_STRING_VALUE;
    val.u.object_value = mas_literal_to_mas_ostring(interp, expr->u.string_value);
    push_value(interp, &val);
    
}

static void call_native_function(MAS_Interpreter* interp, LocalEnvironment* env,
        Expression* expr, MAS_NativeFunctionProc proc) {
    fprintf(stderr, "call native function\n");
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
    
    /*
    if (val) {
        fprintf(stderr, "arg exists\n");
        fprintf(stderr, "type = %d\n", val->type);
        fprintf(stderr, "value = %s\n", val->u.object_value->u.string.string);
    }
    */
    
    
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

static Variable* search_global_variable_from_env(MAS_Interpreter* interp, 
        LocalEnvironment* env, char* identifier) {
    if (env == NULL) {
        return MAS_search_global_variable(interp, identifier);
    }
    return NULL;
}
static MAS_Value* lookup_identifier_address(MAS_Interpreter* interp, 
        LocalEnvironment* env, char* identifier) {
    fprintf(stderr, "identifier = %s\n", identifier);
    
    Variable* val = MAS_search_local_variable(env, identifier);
    if (val) { // find in local variable
        return &val->value;
    } else {
        if (env) {
            val = mas_add_local_variable(env, identifier);
        } else {
            val = mas_add_global_variable(interp, identifier);
        }
    }
    
    if (!val) {
        fprintf(stderr, "cannot allocate variable in lookup identifier address\n");
    }
    return &val->value;
}

static MAS_Value* get_lvalue(MAS_Interpreter* interp, 
        LocalEnvironment* env, Expression* l_expr) {
    MAS_Value* l_valp = NULL;
    switch (l_expr->type) {
        case IDENTIFIER_EXPRESSION: {
            l_valp = lookup_identifier_address(interp, env, l_expr->u.identifier);
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
    fprintf(stderr, "assignment expr\n");        
    MAS_Value* l_valp;    
    Expression* l_expr = expr->u.assign_expression.variable;
    Expression* r_expr = expr->u.assign_expression.operand;
    l_valp = get_lvalue(interp, env, l_expr);
    mas_eval_expression(interp, env, r_expr);
    *l_valp = *peek_stack(interp, 0);    
    mas_show_all_global_variable(interp);    
}

static void mas_eval_identifier_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value val;
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
            fprintf(stderr, "function call\n");
            mas_eval_function_call_expression(interp, env, expr);
            break;
        }
        default: {
            fprintf(stderr, "undefined expression type in mas_eval_expression %d\n", expr->type);
            exit(1);
        }
    }      
}
