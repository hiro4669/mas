#include <stdio.h>
#include <stdlib.h>
#include "mas.h"


static MAS_Value call_native_function(MAS_Interpreter* interp, LocalEnvironment* env,
        Expression* expr, MAS_NativeFunctionProc proc) {
    MAS_Value v;
    // check argument count
    ArgumentList* arg_list;
    int arg_count;
    for (arg_count = 0, arg_list = expr->u.function_call_expression.argument; arg_list;
            arg_list = arg_list->next, ++arg_count);

    proc(interp, 0, NULL);
    
    return v;
}

static MAS_Value mas_eval_function_call_expression(MAS_Interpreter* interp, 
        LocalEnvironment* env, Expression* expr) {
    MAS_Value v;
    fprintf(stderr, "func name = %s\n", expr->u.function_call_expression.identifier);
    FunctionDefinition* func = mas_search_function(expr->u.function_call_expression.identifier);
    if (func == NULL) {
        mas_runtime_error(expr->line_number,
                FUNCTION_NOT_FOUND_ERR,
                STRING_MESSAGE_ARGUMENT, "name", expr->u.function_call_expression.identifier,
                MESSAGE_ARGUMENT_END);
    }
    
    switch(func->type) {
        case NATIVE_FUNCTION: {
            fprintf(stderr, "call native function\n");
            v = call_native_function(interp, env, expr, func->u.native_f.n_func);
            break;
        }
        default: {  
            fprintf(stderr, "no such func type in %d mas_eval_function_call_expression\n", func->type);
            exit(1);
            break;
        }
            
    }
    
    
    return v;
}

MAS_Value mas_eval_expression(MAS_Interpreter* interp, 
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    
    switch (expr->type) {
        case FUNCTION_CALL_EXPRESSION: {         
            value = mas_eval_function_call_expression(interp, env, expr);
            break;
        }
        default: {
            fprintf(stderr, "undefined expression type in mas_eval_expression %d\n", expr->type);
            exit(1);
        }
    }
    
    
    return value;    
}

