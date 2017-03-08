#include <stdio.h>
#include <stdlib.h>
#include "visitor.h"


static void traverse_expr_children(Expression* expr, Visitor* visitor);


void traverse_stmt(Statement* stmt, Visitor* visitor) {
    if (stmt) {
        if (visitor->enter_stmt_list[stmt->type] == NULL) { // for debugging
            fprintf(stderr, "stmt->type(%d) is null\n", stmt->type);
            exit(1);
        }
        visitor->enter_stmt_list[stmt->type](stmt);
        visitor->leave_stmt_list[stmt->type](stmt);
        

        
    }
}



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
