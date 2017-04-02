#include <stdio.h>
#include <stdlib.h>
#include "mas.h"

static void release_if_string(MAS_Value* v) {
    if (v->type == MAS_STRING_VALUE) {
        mas_release_string(v->u.string_value);
    }
}

static MAS_Value call_native_function(MAS_Interpreter* interp, LocalEnvironment* env,
        Expression* expr, MAS_NativeFunctionProc proc) {
    MAS_Value v;
    // check argument count
    ArgumentList* arg_list;
    int arg_count, i;

    for (arg_count = 0, arg_list = expr->u.function_call_expression.argument; arg_list;
            arg_list = arg_list->next, ++arg_count);
    MAS_Value* args = (MAS_Value*)MEM_malloc(sizeof(MAS_Value) * arg_count);
        
//    fprintf(stderr, "arg_count = %d\n", arg_count);
    
    for (i = 0, arg_list = expr->u.function_call_expression.argument; arg_list;
            arg_list = arg_list->next, ++i) {
        args[i] = mas_eval_expression(interp, env, arg_list->expression);        
    }

    proc(interp, arg_count, args);
    for (i = 0; i < arg_count; ++i) {
        release_if_string(&args[i]);
    }
    MEM_free(args);    
    
    return v;
}

static MAS_Value mas_eval_function_call_expression(MAS_Interpreter* interp, 
        LocalEnvironment* env, Expression* expr) {
    MAS_Value v;
  //  fprintf(stderr, "func name = %s\n", expr->u.function_call_expression.identifier);
    FunctionDefinition* func = mas_search_function(expr->u.function_call_expression.identifier);
    if (func == NULL) {
        mas_runtime_error(expr->line_number,
                FUNCTION_NOT_FOUND_ERR,
                STRING_MESSAGE_ARGUMENT, "name", expr->u.function_call_expression.identifier,
                MESSAGE_ARGUMENT_END);
    }
    
    switch(func->type) {
        case NATIVE_FUNCTION: {
//            fprintf(stderr, "call native function\n");
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

static MAS_Value mas_eval_string_expression(MAS_Interpreter* interp, 
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    value.u.string_value = mas_literal_to_mas_string(interp, expr->u.string_value);
    value.type = MAS_STRING_VALUE;
    return value;
}

static MAS_Value mas_eval_int_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    value.u.int_value = expr->u.int_value;
    value.type = MAS_INT_VALUE;
    return value;
}

static MAS_Value mas_eval_boolean_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    value.u.boolean_value = expr->u.boolean_value;
    value.type = MAS_BOOLEAN_VALUE;    
    return value;
}

MAS_Value mas_eval_expression(MAS_Interpreter* interp, 
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    
    switch (expr->type) {
        case BOOLEAN_EXPRESSION: {
            value = mas_eval_boolean_expression(interp, env, expr);
            break;
        }
        case FUNCTION_CALL_EXPRESSION: {         
            value = mas_eval_function_call_expression(interp, env, expr);
            break;
        }
        case STRING_EXPRESSION: {
            value = mas_eval_string_expression(interp, env, expr);
            break;
        }
        case INT_EXPRESION: {
            value = mas_eval_int_expression(interp, env, expr);
            break;
        }
        default: {
            fprintf(stderr, "undefined expression type in mas_eval_expression %d\n", expr->type);
            exit(1);
        }
    }
    
    
    return value;    
}

