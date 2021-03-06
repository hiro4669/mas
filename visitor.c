#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "visitor.h"

static int depth = 0;

static void increment() {
    depth++;
}

static void decrement() {
    depth--;
}

void print_depth() {
    int i = 0;
    for(i = 0; i < depth; i++) {
        fprintf(stderr, "    ");
    }
}


static void enter_mulexpr(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter mulexpr : *\n");
    increment();
}
static void leave_mulexpr(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave mulexpr\n");
}

static void enter_divexpr(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter divexpr : / \n");
    increment();
}
static void leave_divexpr(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave divexpr\n");
}

static void enter_modexpr(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter modexpr : mod \n");
    increment();
}
static void leave_modexpr(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave modexpr\n");
}


static void enter_addexpr(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter addexpr : +\n");
    increment();
    
}
static void leave_addexpr(Expression* expr) {
    decrement();    
    print_depth();
    fprintf(stderr, "leave addexpr\n");
}

static void enter_subepr(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter subexpr : -\n");
    increment();
}
static void leave_subexpr(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave subexpr\n");
}

static void enter_intexpr(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter intexpr : %d\n", expr->u.int_value);
    increment();
}
static void leave_intexpr(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave intexpr\n");
}

static void enter_doubleexpr(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter doubleexpr : %lf\n", expr->u.double_value);
    increment();
}
static void leave_doubleexpr(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave doubleexpr\n");
}

static void enter_stringexpr(Expression *expr) {
    int i;
    print_depth();
    fprintf(stderr, "enter stringexpr : str = %s, len = %d\n", expr->u.string_value, (int)strlen(expr->u.string_value));
    /*
    for (i = 0; i < strlen(expr->u.string_value); ++i) {
        fprintf(stderr, "0x%x\n", expr->u.string_value[i]);
    }
     */
    
    increment();
}
static void leave_stringexpr(Expression *expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave stringexpr\n");
}

static void enter_nullexpr(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter nullexpr\n");
    increment();
}
static void leave_nullexpr(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave nullexpr\n");
}

static void enter_boolean_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter booleanexpr = %d\n", expr->u.boolean_value);
    increment();
}
static void leave_boolean_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave booleanexpr\n");
}

static void enter_identifier_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter identifierexpr = %s\n", expr->u.identifier);
    increment();
}
static void leave_identifier_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave identifierexpr\n");    
}

static void enter_functioncall_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter functioncall_expr = %s\n", expr->u.function_call_expression.identifier);    
    increment();
}
static void leave_functioncall_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave functioncall_expr\n");  
}

static void enter_minus_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter minusexpr: -\n");
    increment();
}

static void leave_minus_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave minusexpr\n");  
}

static void enter_gt_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter gt expr: >\n");
    increment();
}
static void leave_gt_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave gt expr\n");  
}

static void enter_ge_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter ge expr : >=\n");
    increment();
}
static void leave_ge_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave ge expr\n");  
}

static void enter_lt_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter lt expr: <\n");
    increment();
}
static void leave_lt_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave lt expr\n");  
}

static void enter_le_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter le expr: <=\n");
    increment();
}
static void leave_le_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave le expr\n");  
}

static void enter_eq_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter eq expr: ==\n");
    increment();
}
static void leave_eq_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave eq expr\n");  
}

static void enter_ne_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter ne expr: !=\n");
    increment();
}
static void leave_ne_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave ne expr\n");  
}

static void enter_logical_and_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter logical and expr: &&\n");
    increment();
}
static void leave_logical_and_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave logical and expr\n");  
}

static void enter_logical_or_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter logical or expr: ||\n");
    increment();
}
static void leave_logical_or_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave logical or expr\n");  
}

static void enter_assignment_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter assignment expr : \n");
    increment();
}
static void leave_assignment_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave assignment expr\n");  
}

static void enter_index_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter index expr : \n");
    increment();
}
static void leave_index_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave index expr\n");  
}

static void enter_increment_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter increment expr : \n");
    increment();
}
static void leave_increment_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave increment expr : \n");
}

static void enter_decrement_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter decrement expr : \n");
    increment();
}
static void leave_decrement_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave decrement expr : \n");
}

static void enter_methodcall_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter methodcall expr : \n");
    increment();
}
static void leave_methodcall_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave methodcall expr : \n");
}

static void enter_array_expression(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter array expr : \n");
    increment();
}
static void leave_array_expression(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave array expr : \n");
}

/*  Statement */

static void enter_expression_statement(Statement* stmt) {
    print_depth();
    fprintf(stderr, "enter expression stmt\n");
    increment();
}

static void leave_expression_statement(Statement* stmt) {
    decrement();
    print_depth();
    fprintf(stderr, "leave expression stmt\n");
}

static void enter_global_statement(Statement* stmt) {
    print_depth();
    fprintf(stderr, "enter global statement\n");
    increment();
}
static void leave_global_statement(Statement* stmt) {
    decrement();
    print_depth();
    fprintf(stderr, "leave global statement\n");
}

static void enter_return_statement(Statement* stmt) {
    print_depth();
    fprintf(stderr, "enter return statement\n");
    increment();
}
static void leave_return_statement(Statement* stmt) {
    decrement();
    print_depth();
    fprintf(stderr, "leave return statement\n");
}

static void enter_break_statement(Statement* stmt) {
    print_depth();
    fprintf(stderr, "enter break statement\n");
    increment();
}
static void leave_break_statement(Statement* stmt) {
    decrement();
    print_depth();
    fprintf(stderr, "leave break statement\n");
}

static void enter_continue_statement(Statement* stmt) {
    print_depth();
    fprintf(stderr, "enter continue statement\n");
    increment();
}
static void leave_continue_statement(Statement* stmt) {
    decrement();
    print_depth();
    fprintf(stderr, "leave continue statement\n");
}

static void enter_while_statement(Statement* stmt) {
    print_depth();
    fprintf(stderr, "enter while statement\n");
    increment();
}
static void leave_while_statement(Statement* stmt) {
    decrement();
    print_depth();
    fprintf(stderr, "leave while statement\n");
}

static void enter_for_statement(Statement* stmt) {
    print_depth();
    fprintf(stderr, "enter for statement\n");
    increment();
}
static void leave_for_statement(Statement* stmt) {
    decrement();
    print_depth();
    fprintf(stderr, "leave for statement\n");
}

static void enter_if_statement(Statement* stmt) {
    print_depth();
    fprintf(stderr, "enter if statement\n");
    increment();
}
static void leave_if_statement(Statement* stmt) {
    decrement();
    print_depth();
    fprintf(stderr, "leave if statement\n");
}

/* Function */
static void enter_mas_func(FunctionDefinition* func) {
    print_depth();
    fprintf(stderr, "enter mas func : %s\n", func->name);
    increment();
}
static void leave_mas_func(FunctionDefinition* func) {
    decrement();
    print_depth();
    fprintf(stderr, "leave mas func\n");
}

static void enter_native_func(FunctionDefinition* func) {
    print_depth();
    fprintf(stderr, "enter native func : %s\n", func->name);
    increment();
}
static void leave_native_func(FunctionDefinition* func) {
    decrement();
    print_depth();
    fprintf(stderr, "leave native func\n");
}


Visitor* create_visitor() {
    visit_expr* enter_list;
    visit_expr* leave_list;
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list; 
    visit_func* enter_func_list;
    visit_func* leave_func_list;
    

    Visitor* visitor = (Visitor*)malloc(sizeof(Visitor));
    /* Expression list creation */
    enter_list = (visit_expr*)malloc(sizeof(visit_expr) * EXPRESSION_TYPE_COUNT_PLUS_1);
    leave_list = (visit_expr*)malloc(sizeof(visit_expr) * EXPRESSION_TYPE_COUNT_PLUS_1);
    
    /* Statement list creation */
    enter_stmt_list = (visit_stmt*)malloc(sizeof(visit_stmt) * STATEMENT_TYPE_COUNT_PLUS_1);
    leave_stmt_list = (visit_stmt*)malloc(sizeof(visit_stmt) * STATEMENT_TYPE_COUNT_PLUS_1);
    
    /* Function list creation */
    enter_func_list = (visit_func*)malloc(sizeof(visit_func) * FUNCTION_TYPE_PLUS1);
    leave_func_list = (visit_func*)malloc(sizeof(visit_func) * FUNCTION_TYPE_PLUS1);
    
    enter_list[MUL_EXPRESSION]           = enter_mulexpr;
    enter_list[DIV_EXPRESSION]           = enter_divexpr;
    enter_list[MOD_EXPRESSION]           = enter_modexpr;
    enter_list[ADD_EXPRESSION]           = enter_addexpr;
    enter_list[SUB_EXPRESSION]           = enter_subepr;
    enter_list[INT_EXPRESSION]            = enter_intexpr;
    enter_list[DOUBLE_EXPRESSION]        = enter_doubleexpr;
    enter_list[STRING_EXPRESSION]        = enter_stringexpr;
    enter_list[NULL_EXPRESSION]          = enter_nullexpr;
    enter_list[BOOLEAN_EXPRESSION]       = enter_boolean_expression;
    enter_list[IDENTIFIER_EXPRESSION]    = enter_identifier_expression;
    enter_list[FUNCTION_CALL_EXPRESSION] = enter_functioncall_expression;
    enter_list[MINUS_EXPRESSION]         = enter_minus_expression;
    enter_list[GT_EXPRESSION]            = enter_gt_expression;
    enter_list[GE_EXPRESSION]            = enter_ge_expression;
    enter_list[LT_EXPRESSION]            = enter_lt_expression;
    enter_list[LE_EXPRESSION]            = enter_le_expression;
    enter_list[EQ_EXPRESSION]            = enter_eq_expression;
    enter_list[NE_EXPRESSION]            = enter_ne_expression;
    enter_list[LOGICAL_AND_EXPRESSION]   = enter_logical_and_expression;
    enter_list[LOGICAL_OR_EXPRESSION]    = enter_logical_or_expression;    
    enter_list[ASSIGN_EXPRESSION]        = enter_assignment_expression;
    enter_list[INDEX_EXPRESSION]         = enter_index_expression;
    enter_list[INCREMENT_EXPRESSION]     = enter_increment_expression;
    enter_list[DECREMENT_EXPRESSION]     = enter_decrement_expression;
    enter_list[METHOD_CALL_EXPRESSION]   = enter_methodcall_expression;
    enter_list[ARRAY_EXPRESSION]         = enter_array_expression;
        
    leave_list[MUL_EXPRESSION]           = leave_mulexpr;
    leave_list[DIV_EXPRESSION]           = leave_divexpr;
    leave_list[MOD_EXPRESSION]           = leave_modexpr;
    leave_list[ADD_EXPRESSION]           = leave_addexpr;
    leave_list[SUB_EXPRESSION]           = leave_subexpr;
    leave_list[INT_EXPRESSION]            = leave_intexpr;
    leave_list[DOUBLE_EXPRESSION]        = leave_doubleexpr;
    leave_list[STRING_EXPRESSION]        = leave_stringexpr;
    leave_list[NULL_EXPRESSION]          = leave_nullexpr;
    leave_list[BOOLEAN_EXPRESSION]       = leave_boolean_expression;
    leave_list[IDENTIFIER_EXPRESSION]    = leave_identifier_expression;
    leave_list[FUNCTION_CALL_EXPRESSION] = leave_functioncall_expression;
    leave_list[MINUS_EXPRESSION]         = leave_minus_expression;
    leave_list[GT_EXPRESSION]            = leave_gt_expression;
    leave_list[GE_EXPRESSION]            = leave_ge_expression;
    leave_list[LT_EXPRESSION]            = leave_lt_expression;
    leave_list[LE_EXPRESSION]            = leave_le_expression;    
    leave_list[EQ_EXPRESSION]            = leave_eq_expression;
    leave_list[NE_EXPRESSION]            = leave_ne_expression;
    leave_list[LOGICAL_AND_EXPRESSION]   = leave_logical_and_expression;
    leave_list[LOGICAL_OR_EXPRESSION]    = leave_logical_or_expression;
    leave_list[ASSIGN_EXPRESSION]        = leave_assignment_expression;
    leave_list[INDEX_EXPRESSION]         = leave_index_expression;
    leave_list[INCREMENT_EXPRESSION]     = leave_increment_expression;
    leave_list[DECREMENT_EXPRESSION]     = leave_decrement_expression;
    leave_list[METHOD_CALL_EXPRESSION]   = leave_methodcall_expression;
    leave_list[ARRAY_EXPRESSION]         = leave_array_expression;
    
    /* Statement */    
    enter_stmt_list[EXPRESSION_STATEMENT] = enter_expression_statement;
    enter_stmt_list[GLOBAL_STATEMENT]     = enter_global_statement;
    enter_stmt_list[RETURN_STATEMENT]     = enter_return_statement;
    enter_stmt_list[BREAK_STATEMENT]      = enter_break_statement;
    enter_stmt_list[CONTINUE_STATEMENT]   = enter_continue_statement;
    enter_stmt_list[WHILE_STATEMENT]      = enter_while_statement;
    enter_stmt_list[FOR_STATEMENT]        = enter_for_statement;
    enter_stmt_list[IF_STATEMENT]         = enter_if_statement;
    
    leave_stmt_list[EXPRESSION_STATEMENT] = leave_expression_statement;
    leave_stmt_list[GLOBAL_STATEMENT]     = leave_global_statement;
    leave_stmt_list[RETURN_STATEMENT]     = leave_return_statement;
    leave_stmt_list[BREAK_STATEMENT]      = leave_break_statement;
    leave_stmt_list[CONTINUE_STATEMENT]   = leave_continue_statement;
    leave_stmt_list[WHILE_STATEMENT]      = leave_while_statement;
    leave_stmt_list[FOR_STATEMENT]        = leave_for_statement;
    leave_stmt_list[IF_STATEMENT]         = leave_if_statement;
    
    /* Function */
    enter_func_list[MAS_FUNCTION]    = enter_mas_func;
    enter_func_list[NATIVE_FUNCTION] = enter_native_func;
    
    leave_func_list[MAS_FUNCTION]    = leave_mas_func;
    leave_func_list[NATIVE_FUNCTION] = leave_native_func;
    
    
    
    visitor->enter_list = enter_list;
    visitor->leave_list = leave_list;
    
    visitor->enter_stmt_list = enter_stmt_list;
    visitor->leave_stmt_list = leave_stmt_list;
    
    visitor->enter_func_list = enter_func_list;
    visitor->leave_func_list = leave_func_list;
    
    return visitor;
}

/*
int main(void) {
    Visitor* visitor = create_visitor();
    fprintf(stderr, "visitor = %p\n", visitor);
    visitor->enter_list[ADD_EXPRESSION](NULL);
    
    return 0;
}
*/


