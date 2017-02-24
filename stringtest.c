#include <stdio.h>
#include "mas.h"

int main(void) {
    char *message = "Hello World";
    char *copy = NULL;
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
    
    copy = mas_create_identifier(message);
    fprintf(stderr, "copy = %s\n", copy);
    
    mas_reset_string_literal();
    return 0;
}
