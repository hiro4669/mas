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
    result.u.return_value = val;
    return result;
}

static StatementResult execute_while_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    Expression* condexpr = stmt->u.while_s.condexpr;
    while(1) {    
        MAS_Value cv = mas_eval_expression_with_ret(interp, env, condexpr);
        if (cv.type != MAS_BOOLEAN_VALUE) {
            mas_runtime_error(condexpr->line_number,
                    NOT_BOOLEAN_OPERATOR_ERR, MESSAGE_ARGUMENT_END);
        }
        if (cv.u.boolean_value == MAS_TRUE) {
            result = mas_execute_statementlist(interp, env, 
                    stmt->u.while_s.block->stmt_list);
            
            if (result.type == BREAK_STATEMENT_RESULT) { // break
                result.type = NORMAL_STATEMENT_RESULT;
                result.u.return_value.type = MAS_NULL_VALUE;
                return result;
            }
            if (result.type == RETURN_STATEMENT_RESULT) {
                return result;
            }
            
        } else {
            break;
        }
    }
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
        case WHILE_STATEMENT: {
            result = execute_while_statement(interp, env, stmt);
            break;
        }
        default: {
            fprintf(stderr, "undefined stmt type in mas_execute_statement(exec.c) %d\n", stmt->type);
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
//        fprintf(stderr, "\nexec statement\n");
        result = mas_execute_statement(interp, env, pos->statement);
//        mas_run_gc(interp);
        
    }           
    return result;    
}