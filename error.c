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



static void init_v_string(VString* v) {
    v->string = NULL;
}
static void clear_v_string(VString* v) {
    if (v->string) {
        MEM_free(v->string);
        v->string = NULL;
    }
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
                args[idx].u.pointer_val = va_arg(ap, void*);
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

static void show_args(const MessageArgument* args) {
    int i = 0;
    while(args[i].type != MESSAGE_ARGUMENT_END) {
        fprintf(stderr, "type:%d, name:%s val:", args[i].type, args[i].name);

        switch(args[i].type) {
            case INT_MESSAGE_ARGUMENT:
                fprintf(stderr, "%d\n", args[i].u.int_val);
                break;
            case STRING_MESSAGE_ARGUMENT:
                fprintf(stderr, "%s\n", args[i].u.str_val);
                break;
            default:
                break;
        }

        ++i;
    }
}

static int search_args(const MessageArgument* args, MessageArgument* target, const char* name) {
    int i = 0, r = 0;
    while(args[i].type != MESSAGE_ARGUMENT_END) {
        if (!strcmp(name, args[i].name)) { // equels
            r = 1;
            *target = args[i];
            break;
        }
        ++i;        
    }    
    return r;
}

void create_message(VString* v, int len, MessageArgument* args, const MessageFormat msg) {
    int i, j, k;
    int r;
    char buffer[256] = {0};
    int str_len = strlen(msg.format);
    init_v_string(v);
    MessageArgument cur_arg;
    
    for (i = 0; i < str_len; ++i) {
        if (msg.format[i] == '$') {            
            for(k = 0, j = i+2;; ++j, ++k) {
                if (msg.format[j] == ')') break;
                buffer[k] = msg.format[j];
            }
            buffer[k] = 0;
//            fprintf(stderr, "serach : %s\n", buffer);
            r = search_args(args, &cur_arg, buffer);
            if (r) { // find
//                fprintf(stderr, "find!\n");
//                fprintf(stderr, "type = %d, name = %s\n", cur_arg.type, cur_arg.name);
                switch(cur_arg.type) {
                    case INT_MESSAGE_ARGUMENT:
                        sprintf(buffer, "%d", cur_arg.u.int_val);
                        break;
                    case STRING_MESSAGE_ARGUMENT:
                        sprintf(buffer, "%s", cur_arg.u.str_val);
                        break;
                    case DOUBLE_MESSAGE_ARGUMENT:
                        sprintf(buffer, "%f", cur_arg.u.double_val);
                        break;
                    case CHARACTER_MESSAGE_ARGUMENT:
                        sprintf(buffer, "%c", cur_arg.u.char_val);
                        break;
                    case POINTER_MESSAGE_ARGUMENT:
                        sprintf(buffer, "0x%p", cur_arg.u.pointer_val);
                        break;
                    default:
                        break;
                }
//                fprintf(stderr, "val = %s\n", buffer);
                add_string(v, buffer); // add string
                i = j;                
  //              fprintf(stderr, "i = %d, c = %c\n", i, msg.format[i]);
            }
            
        } else {
            add_character(v, msg.format[i]);
        }
    }
    fprintf(stderr, "\n");
    
    
    
}

void mas_compile_error(CompileError id, ...) {
    int idx;
    VString message;
    MessageArgument args[MESSAGE_MAX];
    CompileError etype;
    
    va_list ap;
    va_start(ap, id);
    idx = create_argument_list(args, ap);
    create_message(&message, idx, args, mas_compile_error_message_format[id]);
//    fprintf(stderr, "message = %s\n", message.string);
    fprintf(stderr, "%s\n", message.string);
    va_end(ap);
    MEM_free(message.string);
    exit(1);
}

int main(void) {
    VString message;
    init_v_string(&message);
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
    
    mas_compile_error(FUNCTION_MULTIPLE_DEFINE_ERR,
            STRING_MESSAGE_ARGUMENT, "name", "ftest", 
            MESSAGE_ARGUMENT_END);

    MEM_free(message.string);
    MEM_dump_memory();

}