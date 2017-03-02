/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "mas.h"

static void* ast_malloc(size_t size) {
    MAS_Interpreter *interp = mas_get_interpreter();
    return mas_malloc(interp->ast_storage, size);
}

static Expression* mas_alloc_expression(ExpressionType type) {
    Expression* expr = ast_malloc(sizeof(Expression));
    expr->type = type;
    return expr;
}

Expression* mas_create_binary_expression(ExpressionType type,
        Expression* left, Expression* right) {
    Expression* bexpr = mas_alloc_expression(type);
    bexpr->u.binary_expression.left = left;
    bexpr->u.binary_expression.right = right;    
    return bexpr;
}

Expression* mas_create_int_expression(int i) {
    Expression* iexpr= mas_alloc_expression(INT_EXPRESION);
    iexpr->u.int_value = i;
    return iexpr;
}

Expression* mas_create_double_expression(double d) {
    Expression* dexpr= mas_alloc_expression(DOUBLE_EXPRESSION);
    dexpr->u.double_value = d;
    return dexpr;
}

Expression* mas_create_string_expression(char* str) {
    Expression* expr = mas_alloc_expression(STRING_EXPRESSION);
    expr->u.string_value = str;
    return expr;
}

Expression* mas_create_boolean_expression(MAS_Boolean v) {
    Expression* expr = mas_alloc_expression(BOOLEAN_EXPRESSION);
    expr->u.boolean_value = v;
    return expr;
}

Expression* mas_create_null_expression() {
    Expression* expr = mas_alloc_expression(NULL_EXPRESSION);
    return expr;
}

Expression* mas_create_identifier_expression(char* identifier) {
    Expression* expr = mas_alloc_expression(IDENTIFIER_EXPRESSION);
    expr->u.identifier = identifier;
    return expr;
}


