#include <stdio.h>
#include <string.h>
#include "mas.h"
#include "./memory/MEM.h"

int main(void) {
    MAS_Interpreter* interp;
    MAS_String* mstr;
    MAS_String* mstr2;
    char* str = "test";
    char* str2;
    interp = mas_create_interpreter();
    
    str2 = (char*)MEM_malloc(5);
    mstr = mas_literal_to_mas_string(interp, str);
    mas_release_string(mstr);
    strcpy(str2, str);
    mstr2 = mas_create_mas_string(interp, str2);
    fprintf(stderr, "str2 = %s\n", str2);
    mas_release_string(mstr2);
    
    
    mas_delete_interpreter();
    MEM_dump_memory();
    return 0;
}