#include <stdio.h>
#include <stdlib.h>

#include "mas.h"

MAS_Value mas_nv_print(MAS_Interpreter* interp, int arg_count, MAS_Value* args) {
    MAS_Value v;
    v.type = MAS_NULL_VALUE;
    if (arg_count < 1) {
        mas_runtime_error(0, ARGUMENT_TOO_FEW_ERR,
                MESSAGE_ARGUMENT_END);
    }    
    if (arg_count > 1) {
        mas_runtime_error(0, ARGUMENT_TOO_MANY_ERR,
                MESSAGE_ARGUMENT_END);
    }
    switch (args[0].type) {
        case MAS_STRING_VALUE: {
            printf("%s", args[0].u.string_value->string);
            break;
        }
        case MAS_INT_VALUE: {
            printf("%d", args[0].u.int_value);
            break;
        }
        case MAS_BOOLEAN_VALUE: {
            if (args[0].u.boolean_value == MAS_TRUE) {
                printf("true");
            } else {
                printf("false");
            }
            break;
        }
        default: {
            fprintf(stderr, "no mas type(%d) in mas_nv_print\n", args[0].type);
            exit(1);
            break;
        }
    }

    
    
    return v;
}

