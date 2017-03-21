#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "./memory/MEM.h"
#include "mas.h"

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

void mas_compile_error(CompileError id, ...) {
    fprintf(stderr, "compile error\n");
    va_list ap;
    va_start(ap, id);
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
    
    mas_compile_error(PARSE_ERR);

    MEM_dump_memory();
    MEM_free(message.string);

}