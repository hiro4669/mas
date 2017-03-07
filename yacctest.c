#include <stdio.h>
#include <stdlib.h>
#include "mas.h"

#include "info.h"
#include "./memory/MEM.h"

int main(int argc, char* argv[]) {
    MAS_Interpreter* interp;
    Visitor* visitor = create_visitor();
    char* file = "yacctest.ma";
    fprintf(stderr, "argc = %d\n", argc);
    if (argc > 1) {
        file = argv[1];
    }
    
    extern int yyparse(void);
    extern FILE *yyin;
    
    mas_create_interpreter();
    yyin = fopen(file, "r");    
    if (yyparse()) {
        fprintf(stderr, "Error Error Error :line %d\n", mas_get_localinfo()->line_number);
        exit(1);
    }
    fclose(yyin);
    interp = mas_get_interpreter();
    fprintf(stderr, "-- visitor traverse ---\n");
    if (interp->expression) {
        traverse_expr(interp->expression, visitor);
    } else {
        fprintf(stderr, "no expression");
    }
    
    
    mas_reset_string_literal();
    mas_delete_interpreter();
    MEM_dump_memory();
    
    
    return 0;
}