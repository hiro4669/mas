#include <stdio.h>
#include <stdlib.h>

#include "visitor.h"

static int depth = 0;

static void increment() {
    depth++;
}

static void decrement() {
    depth--;
}

static void print_depth() {
    int i = 0;
    for(i = 0; i < depth; i++) {
        fprintf(stderr, "    ");
    }
}


static void enter_mulexpr(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter mulexpr\n");
    increment();
}
static void leave_mulexpr(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave mulexpr\n");
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
    print_depth();
    fprintf(stderr, "enter stringexpr : %s\n", expr->u.string_value);
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


Visitor* create_visitor() {
    visit_expr* enter_list;
    visit_expr* leave_list;
    
    Visitor* visitor = (Visitor*)malloc(sizeof(Visitor));
    enter_list = (visit_expr*)malloc(sizeof(visit_expr) * EXPRESSION_TYPE_COUNT_PLUS_1);
    leave_list = (visit_expr*)malloc(sizeof(visit_expr) * EXPRESSION_TYPE_COUNT_PLUS_1);
    enter_list[MUL_EXPRESSION]     = enter_mulexpr;
    enter_list[ADD_EXPRESSION]     = enter_addexpr;
    enter_list[SUB_EXPRESSION]     = enter_subepr;
    enter_list[INT_EXPRESION]      = enter_intexpr;
    enter_list[DOUBLE_EXPRESSION]  = enter_doubleexpr;
    enter_list[STRING_EXPRESSION]  = enter_stringexpr;
    enter_list[NULL_EXPRESSION]    = enter_nullexpr;
    enter_list[BOOLEAN_EXPRESSION] = enter_boolean_expression;
    enter_list[IDENTIFIER_EXPRESSION] = enter_identifier_expression;    
    
    
    leave_list[MUL_EXPRESSION]     = leave_mulexpr;
    leave_list[ADD_EXPRESSION]     = leave_addexpr;
    leave_list[SUB_EXPRESSION]     = leave_subexpr;
    leave_list[INT_EXPRESION]      = leave_intexpr;
    leave_list[DOUBLE_EXPRESSION]  = leave_doubleexpr;
    leave_list[STRING_EXPRESSION]  = leave_stringexpr;
    leave_list[NULL_EXPRESSION]    = leave_nullexpr;
    leave_list[BOOLEAN_EXPRESSION] = leave_boolean_expression;
    leave_list[IDENTIFIER_EXPRESSION] = leave_identifier_expression;
    
    visitor->enter_list = enter_list;
    visitor->leave_list = leave_list;
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


