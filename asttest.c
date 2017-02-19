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
            break;
        case INT_EXPRESION:
            break;
        case DOUBLE_EXPRESSION:
            break;
        default:
            break;
            
    }
}

int main(void) {
    
    MAS_Interpreter *interp = mas_create_interpreter();
    Expression* int_expr = mas_create_int_expression(10);
    Expression* d_expr = mas_create_double_expression(3.5);
    Expression* bexpr = mas_create_binary_expression(ADD_EXPRESSION, int_expr, d_expr);
    show(bexpr);
    
    mas_delete_interpreter();
    return 0;
}
