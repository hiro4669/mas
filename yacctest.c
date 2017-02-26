#include <stdio.h>
#include <stdlib.h>
#include "mas.h"

#include "info.h"
#include "./memory/MEM.h"

int main(void) {
    
    extern int yyparse(void);
    extern FILE *yyin;
    
    //mas_init_localinfo();
    mas_create_interpreter();
    yyin = fopen("yacctest.ma", "r");
    if (yyparse()) {
        fprintf(stderr, "Error Error Error :line %d\n", mas_get_localinfo()->line_number);
        exit(1);
    }
    fclose(yyin);
//    mas_delete_localinfo();
    mas_delete_interpreter();
    
    return 0;
}