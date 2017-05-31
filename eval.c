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