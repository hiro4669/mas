#include <stdio.h>
#include <stdlib.h>
#include "visitor.h"


static void traverse_expr_children(Expression* expr, Visitor* visitor);

void traverse_expr(Expression* expr, Visitor* visitor) {
    if (expr) {
        visitor->enter_list[expr->type](expr);
        traverse_expr_children(expr, visitor);
        visitor->leave_list[expr->type](expr);
    }
    
}

static void traverse_expr_children(Expression* expr, Visitor* visitor) {
    switch(expr->type) {
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
        default: {
            fprintf(stderr, "not decided ExpressionType %d\n", expr->type);
            exit(1);
            break;
        }
    }
}
