#include <stdio.h>
#include <stdlib.h>

#include "mas.h"
#include "./memory/MEM.h"


int main(int argc, char* argv[]) {
    
    MAS_Interpreter* interp;
    FILE* fp;
    if (argc < 2) {
        fprintf(stderr, "specify soruce file\n");
        exit(1);
    }
    fp = fopen(argv[1], "r");
    interp = mas_create_interpreter();
    MAS_compile(interp, fp);    
    mas_traverse_test(); // just test
    //    mas_execute_statementlist(interp, NULL, interp->stmt_list);
    //    printf("exec addr = %p\n", interp->execution_storage);
    MAS_interpret(interp);
    mas_delete_interpreter();
    MEM_dump_memory();
    
    return 0;
}

