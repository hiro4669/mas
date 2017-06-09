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

static StatementResult execute_for_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    Expression* bexpr = stmt->u.for_s.bexpr;
    Expression* cexpr = stmt->u.for_s.cexpr;
    Expression* iexpr = stmt->u.for_s.iexpr;
    
    if (bexpr) {
        mas_eval_expression_with_ret(interp, env, bexpr);
    }
    
    MAS_Value cv;
    cv.type = MAS_BOOLEAN_VALUE;
    cv.u.boolean_value = MAS_TRUE;
    while(1) {
        if (cexpr) {
            cv = mas_eval_expression_with_ret(interp, env, cexpr);
            if (cv.type != MAS_BOOLEAN_VALUE) {
                mas_runtime_error(cexpr->line_number,
                        NOT_BOOLEAN_OPERATOR_ERR, MESSAGE_ARGUMENT_END);
            }
        }
        if (cv.u.boolean_value == MAS_TRUE) {
            result = mas_execute_statementlist(interp, env,
                    stmt->u.for_s.block->stmt_list);
            if (result.type == BREAK_STATEMENT_RESULT) {
                result.type = NORMAL_STATEMENT_RESULT;
                result.u.return_value.type = MAS_NULL_VALUE;
                return result;
            }
            if (result.type == RETURN_STATEMENT_RESULT) {
                return result;
            }                                    
            if (iexpr) { // iterative expression
                mas_eval_expression_with_ret(interp, env, iexpr);
            }
        } else {
            break;
        }        
    }    
    
    return result;
}

static uint8_t execute_elsif(MAS_Interpreter* interp, LocalEnvironment* env, Elsif* elsif, StatementResult* r) {
    Elsif* pos;
    for (pos = elsif; pos; pos = pos->next) {
        MAS_Value cv = mas_eval_expression_with_ret(interp, env, pos->cexpr);
        if (cv.type != MAS_BOOLEAN_VALUE) {
            mas_runtime_error(pos->cexpr->line_number,
                    NOT_BOOLEAN_OPERATOR_ERR, MESSAGE_ARGUMENT_END);
        }
        if (cv.u.boolean_value == MAS_TRUE) {
            *r = mas_execute_statementlist(interp, env, pos->block->stmt_list);
            return 1;
        }
    }
                
    return 0;
}

static StatementResult execute_if_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    Expression* condexpr = stmt->u.if_s.condexpr;
    MAS_Value cv = mas_eval_expression_with_ret(interp, env, condexpr);
    if (cv.type != MAS_BOOLEAN_VALUE) {
        mas_runtime_error(condexpr->line_number,
                NOT_BOOLEAN_OPERATOR_ERR, MESSAGE_ARGUMENT_END);
    }
    
    if (cv.u.boolean_value == MAS_TRUE) {
        result = mas_execute_statementlist(interp, env, stmt->u.if_s.thenblock->stmt_list);
        return result;
    } else {
        Elsif* elsif = stmt->u.if_s.elsif;
        if (elsif) {
            if (execute_elsif(interp, env, elsif, &result)) {
                return result;
            }
        } 
        if (stmt->u.if_s.elseblock) {
            result = mas_execute_statementlist(interp, env, stmt->u.if_s.elseblock->stmt_list);
            return result;
        }
    }
    return result;
}

static StatementResult execute_break_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    result.type = BREAK_STATEMENT_RESULT;
    return result;
}

static StatementResult execute_continue_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    result.type = CONTINUE_STATEMENT_RESULT;
    return result;
}

static StatementResult execute_return_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    Expression* expr;
    result.u.return_value.type = MAS_NULL_VALUE;
    
    result.type = RETURN_STATEMENT_RESULT;
    if ((expr = stmt->u.return_s.return_value)) {
        MAS_Value v = mas_eval_expression_with_ret(interp, env, expr);
        result.u.return_value = v;
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
        case FOR_STATEMENT: {
            result = execute_for_statement(interp, env, stmt);
            break;
        }
        case IF_STATEMENT: {
            result = execute_if_statement(interp, env, stmt);
            break;
        }
        case BREAK_STATEMENT: {
            result = execute_break_statement(interp, env, stmt);
            break;
        }
        case CONTINUE_STATEMENT: {
            result = execute_continue_statement(interp, env, stmt);
            break;
        }
        case RETURN_STATEMENT_RESULT: {
            result = execute_return_statement(interp, env, stmt);
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
        switch(result.type) {
            case RETURN_STATEMENT_RESULT:
            case BREAK_STATEMENT_RESULT:
            case CONTINUE_STATEMENT_RESULT: {
                return result;
            }
            default: {
                break;
            }
        }
//        mas_run_gc(interp);        
    }           
    return result;    
}