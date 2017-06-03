#include <stdio.h>
#include <stdlib.h>

#include "mas.h"

static void stacktest(MAS_Interpreter* interp) {
    MAS_Value v1, v2, v3, v4;
    v1.type = MAS_BOOLEAN_VALUE;
    v2.type = MAS_INT_VALUE;
    v3.type = MAS_STRING_VALUE;
    v4.type = MAS_DOUBLE_VALUE;
    
    push_value(interp, &v1);
    push_value(interp, &v2);
    push_value(interp, &v3);
    push_value(interp, &v4);
    
    
    MAS_Value v;
    for (int i = 0; i < 4; ++i) {
        v = pop_value(interp);
        fprintf(stderr, "type = %d\n", v.type);
    }
    
    fprintf(stderr, "sp= %d, type = %d\n", interp->stack.stack_pointer, peek_stack(interp, 0)->type);
}

static StatementResult execute_expression_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    result.type = NORMAL_STATEMENT_RESULT;
    mas_eval_expression(interp, env, stmt->u.expression_s);
    MAS_Value val = pop_value(interp);
    return result;
}


static StatementResult mas_execute_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    result.type = NORMAL_STATEMENT_RESULT;
    
    switch (stmt->type) {
        case EXPRESSION_STATEMENT: {
            result = execute_expression_statement(interp, env, stmt);
            break;
        }
        default: {
            fprintf(stderr, "undefined stmt type in mas_execute_statement(eval.c) %d\n", stmt->type);
            exit(1);                    
        }
        
    }
    
    return result;
}

StatementResult mas_execute_statementlist(MAS_Interpreter *interp,
        LocalEnvironment *env, StatementList *stmt_list) {
    StatementResult result;
    result.type = NORMAL_STATEMENT_RESULT;
    result.u.return_value.type = MAS_NULL_VALUE;
    
    StatementList* pos;
    for (pos = stmt_list; pos; pos = pos->next) {
        result = mas_execute_statement(interp, env, pos->statement);
        
    }           
    return result;    
}