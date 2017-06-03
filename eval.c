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

void mas_eval_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    

    switch (expr->type) {
        case STRING_EXPRESSION: {
            mas_eval_string_expression(interp, env, expr);
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
