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
    fprintf(stderr, "-- traverse statement ---\n");    
    StatementList *slist;
    if (interp->stmt_list) {
        for (slist = interp->stmt_list; slist; slist = slist->next) {
            traverse_stmt(slist->statement, visitor);
        }
    }
    fprintf(stderr, "-- traverse function ---\n");    
    FunctionDefinition* flist; 
    if (interp->func_list) { // traverse function list
        for (flist = interp->func_list; flist; flist = flist->next) {
            traverse_func(flist, visitor);
        }
    }
    
    /*
    if (interp->func_list) {
        for (flist = interp->func_list; flist; flist = flist->next) {
            if (flist->type == NATIVE_FUNCTION) {
                flist->u.native_f.n_func(interp, 3, NULL);
            }
        }
    }
    */
    
    
    mas_reset_string_literal(); // remove string buffer
    mas_delete_interpreter();
    MEM_dump_memory();
    
    
    return 0;
}