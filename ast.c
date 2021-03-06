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
    expr->line_number = mas_get_localinfo()->line_number;
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
    Expression* iexpr= mas_alloc_expression(INT_EXPRESSION);        
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

//Expression* mas_create_assignment_expression(char* identifier, Expression* operand) {
Expression* mas_create_assignment_expression(Expression* identifier, Expression* operand) {    
    Expression* expr = mas_alloc_expression(ASSIGN_EXPRESSION);
    expr->u.assign_expression.variable = identifier;
    expr->u.assign_expression.operand = operand;
    return expr;
}

Expression* mas_create_index_expression(Expression* array, Expression* index) {
    Expression* expr = mas_alloc_expression(INDEX_EXPRESSION);
    expr->u.index_expression.array = array;
    expr->u.index_expression.index = index;
    return expr;
}

Expression* mas_create_incdec_expression(Expression* operand, ExpressionType type) {
    Expression* expr = mas_alloc_expression(type);
    expr->u.inc_dec_expression.operand = operand;
    return expr;
}

Expression* mas_create_methodcall_expression(Expression* expression, char* identifier, ArgumentList* argument) {
    Expression* expr = mas_alloc_expression(METHOD_CALL_EXPRESSION);
    expr->u.method_call_expression.expression = expression;
    expr->u.method_call_expression.identifier = identifier;
    expr->u.method_call_expression.argument   = argument;
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

ExpressionList* mas_create_expression_list(Expression* expr) {
    ExpressionList* expr_list = (ExpressionList*)ast_malloc(sizeof(ExpressionList));
    expr_list->expression = expr;
    expr_list->next = NULL;
    return expr_list;
}

ExpressionList* mas_chain_expression_list(ExpressionList* expr_list, Expression* expr) {
    ExpressionList* pos;
    for (pos = expr_list; pos->next; pos = pos->next);
    pos->next = mas_create_expression_list(expr);
    return expr_list;
}

Expression* mas_create_array_expression(ExpressionList* expr_list) {
    Expression* expr = mas_alloc_expression(ARRAY_EXPRESSION);
    expr->u.array_literal = expr_list;
    return expr;
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

Statement* mas_create_if_statement(Expression* condexpr, Block* thenblock, Elsif* elsif, Block* elseblock) {
    Statement* stmt = mas_alloc_statement(IF_STATEMENT);
    stmt->u.if_s.condexpr = condexpr;
    stmt->u.if_s.thenblock = thenblock;
    stmt->u.if_s.elsif = elsif;
    stmt->u.if_s.elseblock = elseblock;
    
    return stmt;    
}

/* Function Definition */
FunctionDefinition* mas_create_function_definition(char* name, ParameterList* params, Block* block) {
    if (mas_search_function(name)) {
        mas_compile_error(FUNCTION_MULTIPLE_DEFINE_ERR,
                STRING_MESSAGE_ARGUMENT, "name", name,
                MESSAGE_ARGUMENT_END);
    }    
    FunctionDefinition* fdef = (FunctionDefinition*)ast_malloc(sizeof(FunctionDefinition));
    fdef->type = MAS_FUNCTION;
    fdef->name = name;
    fdef->u.mas_f.param = params;
    fdef->u.mas_f.block = block;
    fdef->next = NULL;
    return fdef;
}

FunctionDefinition* mas_chain_function_definition(FunctionDefinition* flist, FunctionDefinition* newf) {
    if (flist == NULL) {
        return newf;
    }
//    printf("aflist is not null\n");

    FunctionDefinition* pos;
    for (pos = flist; pos->next; pos = pos->next);
    pos->next = newf;
    return flist;
}

/* MISC */
Block* mas_create_block(StatementList* stmt_list)  {
    Block* block = (Block*)ast_malloc(sizeof(Block));
    block->stmt_list = stmt_list;    
    return block;
}

IdentifierList* mas_create_identifier_list(char* identifier) {
    IdentifierList* id_list = (IdentifierList*)ast_malloc(sizeof(IdentifierList));
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
Elsif* mas_chain_elsif(Elsif* e_list, Elsif* elsif) {
    Elsif* pos;
    for (pos = e_list; pos->next; pos = pos->next);
    pos->next = elsif;
    return e_list;
}

Elsif* mas_create_elsif(Expression* cexpr, Block* block) {
    Elsif* elsif = (Elsif*)ast_malloc(sizeof(Elsif));
    elsif->cexpr = cexpr;
    elsif->block = block;
    elsif->next = NULL;    
    return elsif;
}

ParameterList* mas_create_parameter(char* name) {
    ParameterList *param = (ParameterList*)ast_malloc(sizeof(ParameterList));
    param->name = name;
    param->next = NULL;
    return param;
}

ParameterList* mas_chain_parameter(ParameterList* plist, char* name) {
    if (plist == NULL) {
        return mas_create_parameter(name);
    }
    ParameterList* pos;
    for(pos = plist; pos->next; pos = pos->next);
    pos->next = mas_create_parameter(name);
    return plist;
}







