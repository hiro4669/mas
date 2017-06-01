#include <stdio.h>
#include <stdlib.h>
#include "mas.h"


void push_value(MAS_Interpreter* interp, MAS_Value* value) {
    if (interp->stack.stack_alloc_size == interp->stack.stack_pointer) {
        interp->stack.stack_alloc_size += STACK_ALLOC_SIZE;
        interp->stack.stack = (MAS_Value*)MEM_realloc(interp->stack.stack, 
                sizeof(MAS_Value) * interp->stack.stack_alloc_size);
        fprintf(stderr, "stack extend realloc\n");
    }
    interp->stack.stack[interp->stack.stack_pointer++] = *value;    
}

MAS_Value pop_value(MAS_Interpreter* interp) {
    if (interp->stack.stack_pointer == 0) {
        fprintf(stderr, "stack is empty\n");
        exit(1);
    }
    return interp->stack.stack[--interp->stack.stack_pointer];
}

MAS_Value* peek_stack(MAS_Interpreter* interp, int index) {
    int pos;
    if ((pos =interp->stack.stack_pointer - index - 1) < 0) {
        fprintf(stderr, "no such value on stack\n");
        exit(1);
    }
    return &interp->stack.stack[pos];
}


static void mas_eval_string_expression(MAS_Interpreter* interp, LocalEnvironment* env, 
        Expression* expr) {
    MAS_Value val;
    val.type = MAS_STRING_VALUE;
    val.u.object_value = mas_literal_to_mas_ostring(interp, expr->u.string_value);
    push_value(interp, &val);
    
}

void mas_eval_expression(MAS_Interpreter* interp,
        LocalEnvironment* env, Expression* expr) {
    

    switch (expr->type) {
        case STRING_EXPRESSION: {
            fprintf(stderr, "string expr\n");
            mas_eval_string_expression(interp, env, expr);
            break;
        }
        default: {
            fprintf(stderr, "undefined expression type in mas_eval_expression %d\n", expr->type);
            exit(1);
        }
    }      
}
