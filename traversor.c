#include <stdio.h>
#include <stdlib.h>
#include "visitor.h"


static void traverse_expr_children(Expression* expr, Visitor* visitor);
static void traverse_stmt_children(Statement* stmt,  Visitor* visitor);
static void traverse_func_children(FunctionDefinition* func, Visitor* visitor);



void traverse_expr(Expression* expr, Visitor* visitor) {
    if (expr) {
        if (visitor->enter_list[expr->type] == NULL) { // for debugging                      
            fprintf(stderr, "expr->type(%d) is null\n", expr->type);
            exit(1);
        }
        visitor->enter_list[expr->type](expr);
        traverse_expr_children(expr, visitor);
        visitor->leave_list[expr->type](expr);
    }    
}

static void traverse_expr_children(Expression* expr, Visitor* visitor) {
    switch(expr->type) {
        case LOGICAL_AND_EXPRESSION:
        case LOGICAL_OR_EXPRESSION:
        case ADD_EXPRESSION:
        case SUB_EXPRESSION:
        case MUL_EXPRESSION:
        case DIV_EXPRESSION:
        case MOD_EXPRESSION:
        case EQ_EXPRESSION:
        case NE_EXPRESSION:
        case GT_EXPRESSION:
        case GE_EXPRESSION:
        case LT_EXPRESSION:
        case LE_EXPRESSION: {
            if (expr->u.binary_expression.left) {
                traverse_expr(expr->u.binary_expression.left, visitor);
            }
            if (expr->u.binary_expression.right) {
                traverse_expr(expr->u.binary_expression.right, visitor);
            }
            break;
        }
        case INT_EXPRESION:
        case DOUBLE_EXPRESSION: 
        case STRING_EXPRESSION:
        case IDENTIFIER_EXPRESSION:
        case BOOLEAN_EXPRESSION: 
        case NULL_EXPRESSION:  {
            break;
        }
        case FUNCTION_CALL_EXPRESSION: {
            ArgumentList* arg_list = NULL;
            for (arg_list = expr->u.function_call_expression.argument; arg_list; arg_list = arg_list->next) {
                traverse_expr(arg_list->expression, visitor);                
            }

            break;
        }
        case MINUS_EXPRESSION: {
            traverse_expr(expr->u.minus_expression, visitor);
            break;
        }
        case ASSIGN_EXPRESSION: {
            traverse_expr(expr->u.assign_expression.operand, visitor);
            break;
        }
        default: {
            fprintf(stderr, "not decided ExpressionType in traverser.c %d\n", expr->type);
            exit(1);
            break;
        }
    }
}


void traverse_stmt(Statement* stmt, Visitor* visitor) {
    if (stmt) {
        if (visitor->enter_stmt_list[stmt->type] == NULL) { // for debugging
            fprintf(stderr, "stmt->type(%d) is null\n", stmt->type);
            exit(1);
        }
        visitor->enter_stmt_list[stmt->type](stmt);
        traverse_stmt_children(stmt, visitor);
        visitor->leave_stmt_list[stmt->type](stmt);               
    }
}

static void traverse_block(Block* block, Visitor* visitor) {
    StatementList* slist;
    if (block->stmt_list) {
        for (slist = block->stmt_list; slist; slist = slist->next) {
            traverse_stmt(slist->statement, visitor);
        }
    }

}

static void traverse_stmt_children(Statement* stmt, Visitor* visitor) {
    switch(stmt->type) {
        case EXPRESSION_STATEMENT: {
            traverse_expr(stmt->u.expression_s, visitor);
            break;
        }
        case GLOBAL_STATEMENT: {
            IdentifierList* i_list;
            for (i_list = stmt->u.global_s.identifier_list; i_list; i_list = i_list->next) {
                fprintf(stderr, "id = %s\n", i_list->identifier);
            }
            break;
        }
        case RETURN_STATEMENT: {
            traverse_expr(stmt->u.return_s.return_value, visitor);
            break;
        }
        case WHILE_STATEMENT: {
            traverse_expr(stmt->u.while_s.condexpr, visitor);
            if (stmt->u.while_s.block) {
                traverse_block(stmt->u.while_s.block, visitor);
            }
            break;
        }
        case FOR_STATEMENT: {
            traverse_expr(stmt->u.for_s.bexpr, visitor);
            traverse_expr(stmt->u.for_s.cexpr, visitor);
            traverse_expr(stmt->u.for_s.iexpr, visitor);
            if (stmt->u.for_s.block) {
                traverse_block(stmt->u.for_s.block, visitor);
            }
            break;
        }
        case IF_STATEMENT: {
            traverse_expr(stmt->u.if_s.condexpr, visitor);
            traverse_block(stmt->u.if_s.thenblock, visitor);
            if (stmt->u.if_s.elsif) {
                Elsif* pos;
                for (pos = stmt->u.if_s.elsif; pos; pos = pos->next) {
                    traverse_expr(pos->cexpr, visitor);
                    traverse_block(pos->block, visitor);
                }
            }
            if (stmt->u.if_s.elseblock) {
                traverse_block(stmt->u.if_s.elseblock, visitor);
            }
            break;
        }
        case BREAK_STATEMENT:
        case CONTINUE_STATEMENT: { // do nothing
            break;
        }            
        default: {
            fprintf(stderr, "not decided StatementType in traverser.c %d\n", stmt->type);
            exit(1);
            break;
        }
    }
}

void traverse_func(FunctionDefinition* func, Visitor* visitor) {

    if (func) {
        if (visitor->enter_func_list[func->type] == NULL) { // for debugging
            fprintf(stderr, "func->type(%d) is null in traverse_func of traverwor.c\n", func->type);
            exit(1);
        }

        visitor->enter_func_list[func->type](func);
        traverse_func_children(func, visitor);
        visitor->leave_func_list[func->type](func);
    }
}

static void traverse_func_children(FunctionDefinition* func, Visitor* visitor) {
    switch(func->type) {
        case MAS_FUNCTION: {
            ParameterList* plist = func->u.mas_f.param;
            for(; plist; plist = plist->next) {
                fprintf(stderr, "%s ", plist->name);
            }
            fprintf(stderr, "\n");
            traverse_block(func->u.mas_f.block, visitor);
            break;            
        }
        case NATIVE_FUNCTION: {                    
            break;
        }
        default:
            break;
    }
}