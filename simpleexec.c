
#include <stdio.h>
#include "mas.h"

static StatementResult mas_execute_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    result.type = NORMAL_STATEMENT_RESULT;
    
    return result;
}

StatementResult mas_execute_statementlist(MAS_Interpreter* interp, 
        LocalEnvironment* env, StatementList* stmt_list) {
    StatementResult result;
    StatementList* pos;
    result.type = NORMAL_STATEMENT_RESULT;
    
    for (pos = interp->stmt_list; pos; pos = pos->next) {
        fprintf(stderr, "yes\n");
    }
    
    return result;
}