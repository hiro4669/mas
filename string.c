
#include <stdio.h>
#include <string.h>

#include "mas.h"

#define STRING_ALLOC_SIZE  (5)

static char* buffer = NULL;
static int str_index = 0;
static int max = 0;

/* Use ast_storage in this case 
 * Change when reuse this module
 */
static void* string_malloc(size_t size) {
    MAS_Interpreter *interp = mas_get_interpreter();
    return mas_malloc(interp->ast_storage, size);
}

void mas_open_string_literal(void) {
    str_index = 0;
}

void mas_add_string_literal(int letter) {
    fprintf(stderr, "letter = %c\n", (char)letter);
    if (max == str_index) {
        fprintf(stderr, "extend memory\n");
        max += STRING_ALLOC_SIZE;
        buffer = MEM_realloc(buffer, STRING_ALLOC_SIZE);
    }
    buffer[str_index++] = letter;
}

void mas_reset_string_literal() {
    MEM_free(buffer);
    buffer = NULL;
    str_index = max = 0;
}

char* mas_close_string_literal() {
    char* new_ptr;
    new_ptr = string_malloc(str_index + 1);
    memcpy(new_ptr, buffer, str_index);
    new_ptr[str_index] = '\0';
    return new_ptr;
}

char* mas_create_identifier(char* str) {
    char* new_str;    
    new_str = string_malloc(strlen(str) + 1);
    strcpy(new_str, str);
    return new_str;
}

/*
int main(void) {
    char* st1;
    char* st2;
    mas_open_string_literal();
    mas_add_string_literal('a');
    mas_add_string_literal('b');
    mas_add_string_literal('c');
    mas_add_string_literal('d');
    mas_add_string_literal('e');
    mas_add_string_literal('x');
    st1 = mas_close_string_literal();
    
    fprintf(stderr, "st1 = %s(%p)\n", st1, st1);

    
    st2 = mas_create_identifier(st1);
    
    fprintf(stderr, "st2 = %s(%p)\n", st2, st2);
    
    mas_reset_string_literal();
    return 0;
}
 */



