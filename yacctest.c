#include <stdio.h>
#include <stdlib.h>
#include "mas.h"

#include "info.h"
#include "./memory/MEM.h"

int main(int argc, char* argv[]) {
    char* file = "yacctest.ma";
    fprintf(stderr, "argc = %d\n", argc);
    if (argc > 1) {
        file = argv[1];
    }
    
    extern int yyparse(void);
    extern FILE *yyin;
    
    //mas_init_localinfo();
    mas_create_interpreter();
//    yyin = fopen("yacctest.ma", "r");
    yyin = fopen(file, "r");    
    if (yyparse()) {
        fprintf(stderr, "Error Error Error :line %d\n", mas_get_localinfo()->line_number);
        exit(1);
    }
    fclose(yyin);
//    mas_delete_localinfo();
    mas_delete_interpreter();
    
    return 0;
}