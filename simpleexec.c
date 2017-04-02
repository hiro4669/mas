
#include <stdio.h>
#include <stdlib.h>
#include "mas.h"

static StatementResult mas_execute_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    result.type = NORMAL_STATEMENT_RESULT;
    switch (stmt->type) {
        case EXPRESSION_STATEMENT: {
            mas_eval_expression(interp, env, stmt->u.expression_s);
            break;
        }
        default: {
            fprintf(stderr, "undefined stmt type in mas_execute_statement %d\n", stmt->type);
            exit(1);
        }
    }    
    
    return result;
}

StatementResult mas_execute_statementlist(MAS_Interpreter* interp, 
        LocalEnvironment* env, StatementList* stmt_list) {
    StatementResult result;
    StatementList* pos;
    result.type = NORMAL_STATEMENT_RESULT;
    
    for (pos = interp->stmt_list; pos; pos = pos->next) {
        mas_execute_statement(interp, env, pos->statement);
    }
    
    return result;
}