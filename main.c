#include <stdio.h>
#include <stdlib.h>

#include "mas.h"
#include "./memory/MEM.h"


int main(int argc, char* argv[]) {
    
    char* fname;
    MAS_Interpreter* interp;
    FILE* fp;
    int mode = 0;
    if (argc < 2) {
        fprintf(stderr, "specify soruce file\n");
        exit(1);
    }
    switch (argc) {
        case 2:
            fname = argv[1];
            break;
        case 3:
            mode = 1;
            fname = argv[2];
            break;
        default: {
            fprintf(stderr, "option error\n");
            exit(1);
        }
    }
    
    
    fp = fopen(fname, "r");
    interp = mas_create_interpreter();
    MAS_compile(interp, fp);    
    mas_traverse_test(); // just test
    
    if (mode) exit(1);
    //    mas_execute_statementlist(interp, NULL, interp->stmt_list);
    //    printf("exec addr = %p\n", interp->execution_storage);
    MAS_interpret(interp);
    mas_delete_interpreter();
    MEM_dump_memory();
    
    return 0;
}

