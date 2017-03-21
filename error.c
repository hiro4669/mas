#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "./memory/MEM.h"
#include "mas.h"

#define MESSAGE_MAX (256)

extern MessageFormat mas_compile_error_message_format[];
extern MessageFormat mas_runtime_error_message_format[];

typedef struct {
    char* string;
} VString;

typedef struct {
    MessageArgumentType type;
    char* name;
    union {
        int    int_val;
        double double_val;
        char*  str_val;
        int    char_val;
        void*  pointer_val;                
    } u;    
} MessageArgument;

int my_strlen(char* str) {
    if (str == NULL) return 0;
    return strlen(str);
}

static void add_character(VString* v, int ch) {
    int current_len;
    current_len = my_strlen(v->string);
    v->string = MEM_realloc(v->string, current_len + 2);
    v->string[current_len] = ch;
    v->string[current_len + 1] = 0;
}

static void add_string(VString* v, char* str) {
    int current_len = my_strlen(v->string);
    v->string = MEM_realloc(v->string, current_len + my_strlen(str) + 1);
    strcpy(&v->string[current_len], str);    
}



static void clear_v_string(VString* v) {
    v->string = NULL;    
}

static int create_argument_list(MessageArgument* args, va_list ap) {
    int idx = 0;
    for (idx = 0; (args[idx].type = va_arg(ap, MessageArgumentType)) != MESSAGE_ARGUMENT_END; ++idx) {
        args[idx].name = va_arg(ap, char*);
        switch(args[idx].type) {
            case INT_MESSAGE_ARGUMENT: {
                args[idx].u.int_val = va_arg(ap, int);
                break;
            }
            case STRING_MESSAGE_ARGUMENT: {
                args[idx].u.str_val = va_arg(ap, char*);
                break;
            }
            case CHARACTER_MESSAGE_ARGUMENT: {
                args[idx].u.char_val = va_arg(ap, int);
                break;
            }
            case DOUBLE_MESSAGE_ARGUMENT: {
                args[idx].u.double_val = va_arg(ap, double);
                break;
            }
            case POINTER_MESSAGE_ARGUMENT: {
                
                break;
            }
            default: {
                fprintf(stderr, "unrecognized MessageArgumentType %d\n", args[idx].type);
                exit(1);
                break;
            }
        }        
    }
//    fprintf(stderr, "idx = %d, type = %d\n", idx, args[idx].type);
//    fprintf(stderr, "type = %d, name = %s, val = %d\n", args[index].type, args[index].name, args[index].u.int_val);
    return idx;
}

void create_message(VString* v, int len, MessageArgument* args, const MessageFormat msg) {
    fprintf(stderr, "message = %s\n", msg.format);
    
    
}

void mas_compile_error(CompileError id, ...) {
    fprintf(stderr, "compile error\n");
    int idx;
    VString message;
    MessageArgument args[MESSAGE_MAX];
    CompileError etype;
    va_list ap;
    va_start(ap, id);
    
    
    idx = create_argument_list(args, ap);
    create_message(&message, idx, args, mas_compile_error_message_format[id]);
    
    va_end(ap);
}

int main(void) {
    VString message;
    clear_v_string(&message);
    add_character(&message, 'a');
    add_character(&message, 'b');
    add_character(&message, 'c');    
    printf("string1 = %s\n", message.string);    
    add_string(&message, "xyz");
    printf("string2 = %s\n", message.string);
    
    mas_compile_error(PARSE_ERR, 
            INT_MESSAGE_ARGUMENT, "token", 10, 
            STRING_MESSAGE_ARGUMENT, "test", "hoge", 
            MESSAGE_ARGUMENT_END);

    MEM_dump_memory();
    MEM_free(message.string);

}