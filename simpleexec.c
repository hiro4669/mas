
#include <stdio.h>
#include <stdlib.h>
#include "mas.h"

static StatementResult execute_expression_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    result.type = NORMAL_STATEMENT_RESULT;
    MAS_Value val = mas_eval_expression(interp, env, stmt->u.expression_s);
    if (val.type == MAS_STRING_VALUE) {
        mas_release_string(val.u.string_value);
    }
    result.u.return_value = val;
    
    return result;
}

static StatementResult execute_while_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    Expression* condexpr = stmt->u.while_s.condexpr;
    
    while(1) {    
        MAS_Value cv = mas_eval_expression(interp, env, condexpr);
        if (cv.type != MAS_BOOLEAN_VALUE) {
            mas_runtime_error(condexpr->line_number,
                NOT_BOOLEAN_OPERATOR_ERR, MESSAGE_ARGUMENT_END);
        }
        if (cv.u.boolean_value == MAS_TRUE) {
            result = mas_execute_statementlist(interp, env, 
                    stmt->u.while_s.block->stmt_list);
            
            if (result.type == BREAK_STATEMENT_RESULT) { // break
                return result;
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
        MAS_Value cv = mas_eval_expression(interp, env, pos->cexpr);
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
    MAS_Value cv = mas_eval_expression(interp, env, condexpr);
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
            if (execute_elsif(interp, env, elsif, &result)) { // if elsif is executed, return 1 otherwise 0
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

static StatementResult mas_execute_statement(MAS_Interpreter* interp,
        LocalEnvironment* env, Statement* stmt) {
    StatementResult result;
    switch (stmt->type) {
        case EXPRESSION_STATEMENT: {
            result = execute_expression_statement(interp, env, stmt);
            break;
        }
        case WHILE_STATEMENT: {
            result = execute_while_statement(interp, env, stmt);
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
    
    for (pos = stmt_list; pos; pos = pos->next) {
        result = mas_execute_statement(interp, env, pos->statement);
        switch (result.type) {
            case BREAK_STATEMENT_RESULT:
            case CONTINUE_STATEMENT_RESULT: {
                return result;
            }
            default: {
                break;
            }
        }
    }
    
    return result;
}