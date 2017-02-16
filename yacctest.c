#include <stdio.h>
#include <stdlib.h>
#include "mas.h"

#include "info.h"
#include "./memory/MEM.h"

int main(void) {
    
    extern int yyparse(void);
    extern FILE *yyin;
    

    LocalInfo *l_info = (LocalInfo*)MEM_malloc(sizeof(LocalInfo));
    l_info->line_number = 10;
    printf("line = %d\n", l_info->line_number);
    
    
    yyin = fopen("yacctest.ma", "r");
    if (yyparse()) {
        fprintf(stderr, "Error Error Error\n");
        exit(1);
    }
    fclose(yyin);
    return 0;
}