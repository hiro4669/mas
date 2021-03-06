#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mas.h"


#define NATIVE_LIB_NAME "mas.lang.file"

static MAS_NativePointerInfo st_native_lib_info = {
    NATIVE_LIB_NAME
};

static void add_ref_in_env(LocalEnvironment* env, MAS_Object* object) {
//    fprintf(stderr, "add_ref_in_env called(%p)\n", object);
    
    RefInNativeFunc* ref = (RefInNativeFunc*)MEM_malloc(sizeof(RefInNativeFunc));
    ref->next = env->ref_in_native;
    env->ref_in_native = ref;
    ref->object = object;
}

MAS_Value mas_nv_fgets(MAS_Interpreter* interp, LocalEnvironment* env, 
        int arg_count, MAS_Value* args) {
    MAS_Value v;
    FILE *fp;
    char buf[LINE_BUF_SIZE];
    char* ret_buf = NULL;
    int ret_len = 0;
    
    if (arg_count < 1) {
        mas_runtime_error(0, ARGUMENT_TOO_FEW_ERR,
                MESSAGE_ARGUMENT_END);
    }
    if (arg_count > 1) {
        mas_runtime_error(0, ARGUMENT_TOO_MANY_ERR,
                MESSAGE_ARGUMENT_END);
    }    
    if (args[0].type != MAS_NATIVE_POINTER_VALUE) {
        mas_runtime_error(0, FGETS_ARGUMENT_TYPE_ERR,
                MESSAGE_ARGUMENT_END);
    }
    
    fp = args[0].u.native_pointer.pointer;
    
    while(fgets(buf, LINE_BUF_SIZE, fp)) {
        int new_len;
        new_len = ret_len + strlen(buf);
        ret_buf = MEM_realloc(ret_buf, new_len + 1);
        if (ret_len == 0) {
            strcpy(ret_buf, buf);
        } else {
            strcat(ret_buf, buf);            
        }
        
        ret_len = new_len;
        if(ret_buf[ret_len - 1] == '\n') {
            break;
        }        
    }
    if (ret_len > 0) {
        v.type = MAS_STRING_VALUE;
        MAS_Object* new_obj = mas_create_mas_ostring(interp, ret_buf);
        add_ref_in_env(env, new_obj);
        v.u.object_value = new_obj;
    } else {
        v.type = MAS_NULL_VALUE;
    }
    return v;
}

MAS_Value mas_nv_fputs(MAS_Interpreter* interp, LocalEnvironment* env, 
        int arg_count, MAS_Value* args) {
    MAS_Value v;
    FILE* fp;
    if (arg_count < 2) {
        mas_runtime_error(0, ARGUMENT_TOO_FEW_ERR,
                MESSAGE_ARGUMENT_END);
    }
    if (arg_count > 2) {
        mas_runtime_error(0, ARGUMENT_TOO_MANY_ERR,
                MESSAGE_ARGUMENT_END);
    }    
    if (args[0].type != MAS_STRING_VALUE || args[1].type != MAS_NATIVE_POINTER_VALUE) {
        mas_runtime_error(0, FPUTS_ARGUMENT_TYPE_ERR,
                MESSAGE_ARGUMENT_END);
    }
//    char* str = args[0].u.string_value->string;
    char* str = args[0].u.object_value->u.string.string;
    
    fp = args[1].u.native_pointer.pointer;
    
    fputs(str, fp);
    
    v.type = MAS_NULL_VALUE;
    return v;
}

MAS_Value mas_nv_open(MAS_Interpreter* interp, LocalEnvironment* env, 
        int arg_count, MAS_Value* args) {
    MAS_Value v;
    if (arg_count < 2) {
        mas_runtime_error(0, ARGUMENT_TOO_FEW_ERR,
                MESSAGE_ARGUMENT_END);
    }
    if (arg_count > 2) {
        mas_runtime_error(0, ARGUMENT_TOO_MANY_ERR,
                MESSAGE_ARGUMENT_END);
    }    
    if (args[0].type != MAS_STRING_VALUE || args[1].type != MAS_STRING_VALUE) {
        mas_runtime_error(0, FOPEN_ARGUMENT_TYPE_ERR,
                MESSAGE_ARGUMENT_END);
    }    
  
    FILE* fp = fopen(args[0].u.object_value->u.string.string,
                     args[1].u.object_value->u.string.string);
            
    if (fp == NULL) {
        exit(1);
        v.type = MAS_NULL_VALUE;
        v.u.native_pointer.pointer = NULL;
    } else {
        v.type = MAS_NATIVE_POINTER_VALUE;
        v.u.native_pointer.info = &st_native_lib_info;
        v.u.native_pointer.pointer = fp;
    }
    
    return v;
}

MAS_Value mas_nv_close(MAS_Interpreter* interp, LocalEnvironment* env, 
        int arg_count, MAS_Value* args) {
    MAS_Value v;
    if (arg_count < 1) {
        mas_runtime_error(0, ARGUMENT_TOO_FEW_ERR,
                MESSAGE_ARGUMENT_END);
    }
    if (arg_count > 1) {
        mas_runtime_error(0, ARGUMENT_TOO_MANY_ERR,
                MESSAGE_ARGUMENT_END);
    }
    if (args[0].type != MAS_NATIVE_POINTER_VALUE) {
        mas_runtime_error(0, FCLOSE_ARGUMENT_TYPE_ERR,
                MESSAGE_ARGUMENT_END);
    }
    
    fclose(args[0].u.native_pointer.pointer);
    v.type = MAS_NULL_VALUE;
    return v;
}


MAS_Value mas_nv_print(MAS_Interpreter* interp, LocalEnvironment* env, 
        int arg_count, MAS_Value* args) {
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
            printf("%s", args[0].u.object_value->u.string.string);
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
        case MAS_DOUBLE_VALUE: {
            printf("%lf", args[0].u.double_value);
            break;
        }
        case MAS_NULL_VALUE: {
            printf("null");
            break;
        }
        case MAS_NATIVE_POINTER_VALUE: {
            printf("(%s:%p)", args[0].u.native_pointer.info->name,
                              args[0].u.native_pointer.pointer);
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

