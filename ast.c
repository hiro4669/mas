/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include "mas.h"
#include "info.h"

static void* ast_malloc(size_t size) {
    MAS_Interpreter *interp = mas_get_interpreter();
    return mas_malloc(interp->ast_storage, size);
}

static Statement* mas_alloc_statement(StatementType type) {
    Statement* stmt = ast_malloc(sizeof(Statement));
    stmt->type = type;
    stmt->line_number = mas_get_localinfo()->line_number;

    return stmt;    
}

static Expression* mas_alloc_expression(ExpressionType type) {
    Expression* expr = ast_malloc(sizeof(Expression));
    expr->type = type;
    return expr;
}

/* For Expression */
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

Expression* mas_create_functioncall_expression(char* identifier, ArgumentList* argument) {
    Expression* expr = mas_alloc_expression(FUNCTION_CALL_EXPRESSION);
    expr->u.function_call_expression.identifier = identifier;
    expr->u.function_call_expression.argument = argument;
    return expr;            
}

Expression* mas_create_minus_expression(Expression* operand) {
    Expression* expr = mas_alloc_expression(MINUS_EXPRESSION);
    expr->u.minus_expression = operand;
    return expr;
}

Expression* mas_create_assignment_expression(char* identifier, Expression* operand) {
    Expression* expr = mas_alloc_expression(ASSIGN_EXPRESSION);
    expr->u.assign_expression.variable = identifier;
    expr->u.assign_expression.operand = operand;
    return expr;
}

ArgumentList* mas_create_argument_list(Expression* expr) {
    ArgumentList* arglist = ast_malloc(sizeof(ArgumentList));
    arglist->expression = expr;
    arglist->next = NULL;    
    return arglist;
}

ArgumentList* mas_chain_argument(ArgumentList* argument, Expression* expr) {
    ArgumentList* pos;
    for (pos = argument; pos->next; pos = pos->next); // go to the last argument
    pos->next = mas_create_argument_list(expr);
    return argument;
}

/*  begin For statement */
Statement* mas_create_expression_statement(Expression* expr) {
    Statement* stmt = mas_alloc_statement(EXPRESSION_STATEMENT);
    stmt->u.expression_s = expr;
    return stmt;
}

Statement* mas_create_global_statement(IdentifierList* identifier_list) {
    Statement* stmt = mas_alloc_statement(GLOBAL_STATEMENT);
    stmt->u.global_s.identifier_list = identifier_list;
    return stmt;
}

StatementList* mas_create_statement_list(Statement* stmt) {
    StatementList* stmt_list = ast_malloc(sizeof(StatementList));
    stmt_list->statement = stmt;
    stmt_list->next = NULL;
    return stmt_list;
}
StatementList* mas_chain_statement_list(StatementList *stmt_list, Statement* stmt) {
    StatementList *pos;
    if (stmt_list == NULL) {
        stmt_list = mas_create_statement_list(stmt);
        return stmt_list;
    }
    for (pos = stmt_list; pos->next; pos = pos->next);
    pos->next = mas_create_statement_list(stmt);
    return stmt_list;    
}

Statement* mas_create_break_statement() {
    Statement* stmt = mas_alloc_statement(BREAK_STATEMENT);
    return stmt;
}

Statement* mas_create_continue_statement() {
    Statement* stmt = mas_alloc_statement(CONTINUE_STATEMENT);
    return stmt;
}

Statement* mas_create_return_statement(Expression* expr) {
    Statement* stmt = mas_alloc_statement(RETURN_STATEMENT);
    stmt->u.return_s.return_value = expr;
    return stmt;  
}

Statement* mas_create_while_statement(Expression* condexpr, Block* block) {
    Statement* stmt = mas_alloc_statement(WHILE_STATEMENT);
    stmt->u.while_s.condexpr = condexpr;
    stmt->u.while_s.block = block;
    return stmt;
}

Statement* mas_create_for_statement(Expression* bexpr, Expression* cexpr, Expression* iexpr, Block* block) {
    Statement* stmt = mas_alloc_statement(FOR_STATEMENT);
    stmt->u.for_s.bexpr = bexpr;
    stmt->u.for_s.cexpr = cexpr;
    stmt->u.for_s.iexpr = iexpr;
    stmt->u.for_s.block = block;
    return stmt;
}

Block* mas_create_block(StatementList* stmt_list)  {
    Block* block = (Block*)ast_malloc(sizeof(Block));
    block->stmt_list = stmt_list;    
    return block;
}

IdentifierList* mas_create_identifier_list(char* identifier) {
    IdentifierList* id_list = ast_malloc(sizeof(IdentifierList));
    id_list->identifier = identifier;
    id_list->next = NULL;    
    return id_list;
}

IdentifierList* mas_chain_identifier_list(IdentifierList* id_list, char* identifier) {
    IdentifierList* pos;
    if (id_list == NULL) {
        return mas_create_identifier_list(identifier);
    }
    for (pos = id_list; pos->next; pos = pos->next);
    pos->next = mas_create_identifier_list(identifier);
    return id_list;    
}






