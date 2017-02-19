/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include "mas.h"

static void show(Expression* expr) {
    switch(expr->type) {
        case ADD_EXPRESSION:
            fprintf(stderr, "add\n");
            show(expr->u.binary_expression.left);
            show(expr->u.binary_expression.right);
            break;
        case SUB_EXPRESSION:
            fprintf(stderr, "sub\n");
            show(expr->u.binary_expression.left);
            show(expr->u.binary_expression.right);
            break;
        case INT_EXPRESION:
            fprintf(stderr, "int:%d\n", expr->u.int_value);
            break;
        case DOUBLE_EXPRESSION:
            fprintf(stderr, "double:%f\n", expr->u.double_value);
            break;
        default:
            break;
            
    }
}

int main(void) {
    
    MAS_Interpreter *interp = mas_create_interpreter();
    Expression* int_expr = mas_create_int_expression(10);
    Expression* d_expr = mas_create_double_expression(3.5);
//    Expression* bexpr = mas_create_binary_expression(ADD_EXPRESSION, int_expr, d_expr);
    
    Expression* int_expr2 = mas_create_int_expression(1);
    Expression* int_expr3 = mas_create_int_expression(2);
    Expression* bexpr2 = mas_create_binary_expression(SUB_EXPRESSION, int_expr2, int_expr3);
    
    Expression* bexpr = mas_create_binary_expression(ADD_EXPRESSION, bexpr2, d_expr);  
    
    show(bexpr);
    
    mas_delete_interpreter();
    return 0;
}
