#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mas.h"


static MAS_String* chain_mas_string(MAS_Interpreter* interp, 
        MAS_String* l_str, MAS_String* r_str ) {
    int n_len, l_len, r_len;
    char* buf;
    MAS_String* n_str;
    
    n_len = (l_len = strlen(l_str->string)) + (r_len =strlen(r_str->string));
    buf = (char*)MEM_malloc(n_len + 1);
    strncpy(buf, l_str->string, l_len);
    strncpy(&buf[l_len], r_str->string, r_len+1);
    
    n_str = mas_create_mas_string(interp, buf);
    mas_release_string(l_str);
    mas_release_string(r_str);
    
    return n_str;
    
}

static void release_if_string(MAS_Value* v) {
    if (v->type == MAS_STRING_VALUE) {
        mas_release_string(v->u.string_value);
    }
}

static void refer_if_string(MAS_Value* v) {
    if (v->type == MAS_STRING_VALUE) {
        mas_refer_string(v->u.string_value);
    }
}

static Variable* search_global_variable_from_env(MAS_Interpreter* interp, LocalEnvironment* env, char* identifier) {
    if (env == NULL) {
        return MAS_search_global_variable(interp, identifier);
    }
    fprintf(stderr, "not implemented yet in search_global_variable_from_env");
    exit(1);
    
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

static MAS_Value mas_eval_double_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    value.u.double_value = expr->u.double_value;
    value.type = MAS_DOUBLE_VALUE;
    return value;
}

static MAS_Value mas_eval_null_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    value.type = MAS_NULL_VALUE;
    return value;
}

static MAS_Value mas_eval_minus_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value = mas_eval_expression(interp, env, expr->u.minus_expression);
    
    if (value.type != MAS_INT_VALUE &&
            value.type != MAS_DOUBLE_VALUE) {
        mas_runtime_error(expr->line_number,
                MINUS_OPERAND_TYPE_ERR,
                MESSAGE_ARGUMENT_END);
    }
    switch (value.type) {
        case MAS_INT_VALUE: {
            value.u.int_value = - value.u.int_value;
            break;
        }
        case MAS_DOUBLE_VALUE: {
            value.u.double_value = - value.u.double_value;
            break;
        }
        default: {
            exit(1);
        }
    }
    return value;
}

MAS_Value mas_eval_multiplicative_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    MAS_Value l_value = mas_eval_expression(interp, env, expr->u.binary_expression.left);
    MAS_Value r_value = mas_eval_expression(interp, env, expr->u.binary_expression.right);
    
    switch (expr->type) {
        case MUL_EXPRESSION: {
            if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_INT_VALUE) { // int * int
                value.type = MAS_INT_VALUE;
                value.u.int_value = l_value.u.int_value * r_value.u.int_value;
            } else if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = l_value.u.int_value * r_value.u.double_value;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_INT_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = l_value.u.double_value * r_value.u.int_value;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = l_value.u.double_value * r_value.u.double_value;
            } else {
                mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", "*",
                        MESSAGE_ARGUMENT_END);
            }
            break;
        }
        case DIV_EXPRESSION: {
            if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_INT_VALUE) { // int * int
                value.type = MAS_INT_VALUE;
                value.u.int_value = (int)(l_value.u.int_value / r_value.u.int_value);
            } else if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = l_value.u.int_value / r_value.u.double_value;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_INT_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = l_value.u.double_value / r_value.u.int_value;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = l_value.u.double_value / r_value.u.double_value;
            } else {
                mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", "/",
                        MESSAGE_ARGUMENT_END);
            }
            break;
        }
        case MOD_EXPRESSION: {
            if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_INT_VALUE) { // int * int
                value.type = MAS_INT_VALUE;
                value.u.int_value = (int)(l_value.u.int_value % r_value.u.int_value);
            } else if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = fmod(l_value.u.int_value, r_value.u.double_value);                
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_INT_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = fmod(l_value.u.double_value, r_value.u.int_value);
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = fmod(l_value.u.double_value, r_value.u.double_value);
            } else {
                mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", "%",
                        MESSAGE_ARGUMENT_END);
            }
            break;
        }
        default: {
            break;
        }        
    }    
    return value;
}

MAS_Value mas_eval_addtive_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    MAS_Value l_value = mas_eval_expression(interp, env, expr->u.binary_expression.left);
    MAS_Value r_value = mas_eval_expression(interp, env, expr->u.binary_expression.right);
    
    switch (expr->type) {
        case ADD_EXPRESSION: {
            
            // in case the left type is string
            if (l_value.type == MAS_STRING_VALUE) {
                value.type = MAS_STRING_VALUE;
                char buf[1024];
                switch(r_value.type) {
                    case MAS_INT_VALUE: {
                        sprintf(buf, "%d", r_value.u.int_value);
                        break;
                    }
                    case MAS_DOUBLE_VALUE: {
                        sprintf(buf, "%f", r_value.u.double_value);
                        break;
                    }
                    case MAS_STRING_VALUE: {
                        MAS_String* n_str = chain_mas_string(interp, l_value.u.string_value, r_value.u.string_value);
                        value.u.string_value = n_str;
                        return value;        // return immediately
                    }
                    case MAS_BOOLEAN_VALUE: {
                        (r_value.u.boolean_value == MAS_FALSE) ?  
                            strncpy(buf, "false", 6) : strncpy(buf, "true", 5);
                        break;
                    }
                    case MAS_NULL_VALUE: {
                        strncpy(buf, "null", 5);
                        break;
                    }
                    default: {
                        fprintf(stderr, "r_value type is not supported in add expression %d\n", r_value.type);
                        mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", "+",
                        MESSAGE_ARGUMENT_END);                        
                        break;
                    }
                }
                char* str = (char*)MEM_malloc(strlen(buf) + 1);
                strncpy(str, buf, strlen(buf)+1);
                MAS_String* r_str = mas_create_mas_string(interp, str);
                MAS_String* n_str = chain_mas_string(interp, l_value.u.string_value, r_str);
                
                value.u.string_value = n_str;
                return value;                                
            }
            
            if (r_value.type == MAS_STRING_VALUE) {
                value.type = MAS_STRING_VALUE;
                char buf[1024];
                switch (l_value.type) {
                    case MAS_INT_VALUE: {
                        sprintf(buf, "%d", l_value.u.int_value);
                        break;
                    }
                    case MAS_DOUBLE_VALUE: {
                        sprintf(buf, "%f", l_value.u.double_value);
                        break;
                    }
                    case MAS_BOOLEAN_VALUE: {
                        (l_value.u.boolean_value == MAS_FALSE) ?  
                            strncpy(buf, "false", 6) : strncpy(buf, "true", 5);
                        break;
                    }
                    case MAS_NULL_VALUE: {
                        strncpy(buf, "null", 5);
                        break;
                    }
                    default: {
                        fprintf(stderr, "l_value type is not supported in add expression\n");
                        mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", "+",
                        MESSAGE_ARGUMENT_END);      
                        break;
                    }
                }
                char* str = (char*)MEM_malloc(strlen(buf) + 1);
                strncpy(str, buf, strlen(buf)+1);
                MAS_String* l_str = mas_create_mas_string(interp, str);
                MAS_String* n_str = chain_mas_string(interp, l_str, r_value.u.string_value);
                
                value.u.string_value = n_str;
                return value;                          
            }
            
            
            if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_INT_VALUE) { // int * int
                value.type = MAS_INT_VALUE;
                value.u.int_value = l_value.u.int_value + r_value.u.int_value;
            } else if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = l_value.u.int_value + r_value.u.double_value;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_INT_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = l_value.u.double_value + r_value.u.int_value;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = l_value.u.double_value + r_value.u.double_value;
            } else {
                mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", "+",
                        MESSAGE_ARGUMENT_END);
            }
            break;
        }
        case SUB_EXPRESSION: {
            if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_INT_VALUE) { // int * int
                value.type = MAS_INT_VALUE;
                value.u.int_value = l_value.u.int_value - r_value.u.int_value;
            } else if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = l_value.u.int_value - r_value.u.double_value;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_INT_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = l_value.u.double_value - r_value.u.int_value;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.type = MAS_DOUBLE_VALUE;
                value.u.double_value = l_value.u.double_value - r_value.u.double_value;
            } else {
                mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", "-",
                        MESSAGE_ARGUMENT_END);
            }
            break;
        }
        default: {
            break;
        }
            
    }    
    return value;
}

MAS_Value mas_eval_relational_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    MAS_Value l_value = mas_eval_expression(interp, env, expr->u.binary_expression.left);
    MAS_Value r_value = mas_eval_expression(interp, env, expr->u.binary_expression.right);
    value.type = MAS_BOOLEAN_VALUE;
    switch (expr->type) {
        case GT_EXPRESSION: {
            if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_INT_VALUE) {
                value.u.boolean_value = (l_value.u.int_value > r_value.u.int_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.u.boolean_value = (l_value.u.int_value > r_value.u.double_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_INT_VALUE) {
                value.u.boolean_value = (l_value.u.double_value > r_value.u.int_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.u.boolean_value = (l_value.u.double_value > r_value.u.double_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_STRING_VALUE && r_value.type == MAS_STRING_VALUE) {
                value.u.boolean_value = (strcmp(l_value.u.string_value->string, r_value.u.string_value->string) > 0) ?
                    MAS_TRUE : MAS_FALSE;
                mas_release_string(l_value.u.string_value);
                mas_release_string(r_value.u.string_value); 
            } else {
                mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", ">",
                        MESSAGE_ARGUMENT_END);
            }
            break;
        }
        case GE_EXPRESSION: {
            if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_INT_VALUE) {
                value.u.boolean_value = (l_value.u.int_value >= r_value.u.int_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.u.boolean_value = (l_value.u.int_value >= r_value.u.double_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_INT_VALUE) {
                value.u.boolean_value = (l_value.u.double_value >= r_value.u.int_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.u.boolean_value = (l_value.u.double_value >= r_value.u.double_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_STRING_VALUE && r_value.type == MAS_STRING_VALUE) {
                value.u.boolean_value = (strcmp(l_value.u.string_value->string, r_value.u.string_value->string) >= 0) ?
                    MAS_TRUE : MAS_FALSE;
                mas_release_string(l_value.u.string_value);
                mas_release_string(r_value.u.string_value);  
            } else {
                mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", ">=",
                        MESSAGE_ARGUMENT_END);
            }
            break;
        }
        case LT_EXPRESSION: {
            if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_INT_VALUE) {
                value.u.boolean_value = (l_value.u.int_value < r_value.u.int_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.u.boolean_value = (l_value.u.int_value < r_value.u.double_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_INT_VALUE) {
                value.u.boolean_value = (l_value.u.double_value < r_value.u.int_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.u.boolean_value = (l_value.u.double_value < r_value.u.double_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_STRING_VALUE && r_value.type == MAS_STRING_VALUE) {
                value.u.boolean_value = (strcmp(l_value.u.string_value->string, r_value.u.string_value->string) < 0) ?                    
                    MAS_TRUE : MAS_FALSE;
                mas_release_string(l_value.u.string_value);
                mas_release_string(r_value.u.string_value);
            } else {
                mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", "<",
                        MESSAGE_ARGUMENT_END);
            }
            break;
        }
        case LE_EXPRESSION: {
            if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_INT_VALUE) {
                value.u.boolean_value = (l_value.u.int_value <= r_value.u.int_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.u.boolean_value = (l_value.u.int_value <= r_value.u.double_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_INT_VALUE) {
                value.u.boolean_value = (l_value.u.double_value <= r_value.u.int_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.u.boolean_value = (l_value.u.double_value <= r_value.u.double_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_STRING_VALUE && r_value.type == MAS_STRING_VALUE) {
                value.u.boolean_value = (strcmp(l_value.u.string_value->string, r_value.u.string_value->string) <= 0) ?
                    MAS_TRUE : MAS_FALSE;
                mas_release_string(l_value.u.string_value);
                mas_release_string(r_value.u.string_value);                
            } else {
                mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", "<=",
                        MESSAGE_ARGUMENT_END);
            }
            break;
        }
        default: {
            break;
        }
    }
    return value;
}

MAS_Value mas_eval_equality_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    MAS_Value l_value = mas_eval_expression(interp, env, expr->u.binary_expression.left);
    MAS_Value r_value = mas_eval_expression(interp, env, expr->u.binary_expression.right);
    
    value.type = MAS_BOOLEAN_VALUE;
    switch(expr->type) {
        case EQ_EXPRESSION: {
            if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_INT_VALUE) {
                value.u.boolean_value = (l_value.u.int_value == r_value.u.int_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.u.boolean_value = (l_value.u.int_value == r_value.u.double_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_INT_VALUE) {
                value.u.boolean_value = (l_value.u.double_value == r_value.u.int_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.u.boolean_value = (l_value.u.double_value == r_value.u.double_value) ? MAS_TRUE : MAS_FALSE;                
            } else if (l_value.type == MAS_STRING_VALUE && r_value.type == MAS_STRING_VALUE) {
                value.u.boolean_value = (strcmp(l_value.u.string_value->string, r_value.u.string_value->string) == 0) ?
                    MAS_TRUE : MAS_FALSE;
                mas_release_string(l_value.u.string_value);
                mas_release_string(r_value.u.string_value);  
            } else if (l_value.type == MAS_NULL_VALUE && r_value.type == MAS_NULL_VALUE) {
                value.u.boolean_value = MAS_TRUE;
            } else {
                mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", "==",
                        MESSAGE_ARGUMENT_END);
            }
            break;
        }
        case NE_EXPRESSION: {
            if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_INT_VALUE) {
                value.u.boolean_value = (l_value.u.int_value != r_value.u.int_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_INT_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.u.boolean_value = (l_value.u.int_value != r_value.u.double_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_INT_VALUE) {
                value.u.boolean_value = (l_value.u.double_value != r_value.u.int_value) ? MAS_TRUE : MAS_FALSE;
            } else if (l_value.type == MAS_DOUBLE_VALUE && r_value.type == MAS_DOUBLE_VALUE) {
                value.u.boolean_value = (l_value.u.double_value != r_value.u.double_value) ? MAS_TRUE : MAS_FALSE;                
            } else if (l_value.type == MAS_STRING_VALUE && r_value.type == MAS_STRING_VALUE) {
                value.u.boolean_value = (strcmp(l_value.u.string_value->string, r_value.u.string_value->string) != 0) ?
                    MAS_TRUE : MAS_FALSE;
                mas_release_string(l_value.u.string_value);
                mas_release_string(r_value.u.string_value);  
            } else if (l_value.type == MAS_NULL_VALUE && r_value.type == MAS_NULL_VALUE) {
                value.u.boolean_value = MAS_FALSE;
            } else {
                mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", "!=",
                        MESSAGE_ARGUMENT_END);
            }
            break;
        }
        default: {
            break;
        }
    }
    
    return value;
}

MAS_Value mas_eval_logical_and_or_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    MAS_Value l_value = mas_eval_expression(interp, env, expr->u.binary_expression.left);
    MAS_Value r_value = mas_eval_expression(interp, env, expr->u.binary_expression.right);
    char* ope;
    if (l_value.type == MAS_BOOLEAN_VALUE && r_value.type == MAS_BOOLEAN_VALUE) {
        value.type = MAS_BOOLEAN_VALUE;        
        switch (expr->type) {
            case LOGICAL_AND_EXPRESSION: {
                value.u.boolean_value = (l_value.u.boolean_value && r_value.u.boolean_value);
                break;
            }
            case LOGICAL_OR_EXPRESSION: {
                value.u.boolean_value = (l_value.u.boolean_value || r_value.u.boolean_value);
                break;
            }
            default: {
                break;
            }
        }
        
    } else {
        ope = (expr->type == LOGICAL_AND_EXPRESSION) ? "&&" : "||";
        mas_runtime_error(expr->line_number,
                        BAD_OPERAND_TYPE_ERR,
                        STRING_MESSAGE_ARGUMENT, "operator", ope,
                        MESSAGE_ARGUMENT_END);
    }
    
    return value;
}

MAS_Value mas_eval_assign_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    char* identifier = expr->u.assign_expression.variable;
    MAS_Value r_value = mas_eval_expression(interp, env, expr->u.assign_expression.operand);
    
    Variable* val = MAS_search_local_variable(env, identifier);
    if (val) {
        MAS_Value p_value = val->value;
        val->value = r_value;
        release_if_string(&p_value);
    } else {
        val = MAS_search_global_variable(interp, identifier);
        if (val) {
            MAS_Value p_value = val->value;
            val->value = r_value;
            release_if_string(&p_value);           
        } else {
            if (env) {
                MAS_add_local_variable(env, identifier, &r_value);
            } else {
                MAS_add_global_variable(interp, identifier, &r_value);
            }
            if (r_value.type == MAS_STRING_VALUE) {
                mas_refer_string(r_value.u.string_value);
            }            
        }
    }        
    return r_value;
}

MAS_Value mas_eval_identifier_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    MAS_Value value;
    fprintf(stderr, "id = %s\n", expr->u.identifier);
    char* identifier = expr->u.identifier;
    Variable* v = NULL;
    v = MAS_search_local_variable(env, identifier);
    if (v) {
        value = v->value;
    } else {
        v = search_global_variable_from_env(interp, env, identifier);
        if (v == NULL) {
             mas_runtime_error(expr->line_number,
                        VARIABLE_NOT_FOUND_ERR,
                        STRING_MESSAGE_ARGUMENT, "name", identifier,
                        MESSAGE_ARGUMENT_END);
        }
        value = v->value;
    }
    
    refer_if_string(&value);
    
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
        case DOUBLE_EXPRESSION: {
            value = mas_eval_double_expression(interp, env, expr);
            break;
        }
        case NULL_EXPRESSION: {
            value = mas_eval_null_expression(interp, env, expr);
            break;
        }
        case MINUS_EXPRESSION: {
            value = mas_eval_minus_expression(interp, env, expr);
            break;
        }
        case MUL_EXPRESSION:
        case DIV_EXPRESSION:
        case MOD_EXPRESSION: {
            value = mas_eval_multiplicative_expression(interp, env, expr);
            break;            
        }
        case ADD_EXPRESSION:
        case SUB_EXPRESSION: {
            value = mas_eval_addtive_expression(interp, env, expr);
            break;
        }
        case GT_EXPRESSION:
        case GE_EXPRESSION:
        case LT_EXPRESSION:
        case LE_EXPRESSION: {
            value = mas_eval_relational_expression(interp, env, expr);
            break;
        }
        case EQ_EXPRESSION:
        case NE_EXPRESSION: {
            value = mas_eval_equality_expression(interp, env, expr);
            break;
        }
        case LOGICAL_OR_EXPRESSION:
        case LOGICAL_AND_EXPRESSION: {
            value = mas_eval_logical_and_or_expression(interp, env, expr);
            break;
        }
        case ASSIGN_EXPRESSION: {
            value = mas_eval_assign_expression(interp, env, expr);
            break;
        }
        case IDENTIFIER_EXPRESSION: {
            value = mas_eval_identifier_expression(interp, env, expr);
            break;
        }
        default: {
            fprintf(stderr, "undefined expression type in mas_eval_expression %d\n", expr->type);
            exit(1);
        }
    }
    
    
    return value;    
}

