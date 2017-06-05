#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mas.h"
#include "./memory/MEM.h"

typedef enum {
    PARSE = 1,
    AST,
    RUN,
    EXEC_TYPE_PLUS_1            
} Exec_Type;


int main(int argc, char* argv[]) {
    
    Exec_Type eType = RUN;
    int i;
    char* fname;
    MAS_Interpreter* interp;
    FILE* fp;
    int mode = 0;
    if (argc < 2) {
        fprintf(stderr, "specify soruce file\n");
        exit(1);
    }
    /*
     * -p: parse
     * -a: parse, create ast and traverse
     */
    for (i = 1; i < (argc-1); ++i) {
        if (!strcmp(argv[i], "-p")) {
            eType = PARSE;
        } else if (!strcmp(argv[i], "-a")) {
            eType = AST;
        } else {
            fprintf(stderr, "option error\n");
            exit(1);
        }
    }
    
    fname = argv[argc-1];
           
    printf("type = %d\n", eType);
    printf("name = %s\n", fname);
        
    fp = fopen(fname, "r");
    interp = mas_create_interpreter();
            
    switch (eType) {
        case PARSE: {
            MAS_compile(interp, fp);
            break;
        }
        case AST: {
            MAS_compile(interp, fp);
            mas_traverse_test(); // just test
            break;
        }
        case RUN: {
            MAS_compile(interp, fp);
            MAS_interpret(interp);
            break;
        }
        default: {
            break;
        }
    }   
    mas_delete_interpreter();
    MEM_dump_memory();
    
    return 0;
}

