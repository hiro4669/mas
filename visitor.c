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
    fprintf(stderr, "enter addexpr\n");
    increment();
    
}

static void leave_addexpr(Expression* expr) {
    decrement();    
    print_depth();
    fprintf(stderr, "leave addexpr\n");
}

static void enter_subepr(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter subexpr\n");
    increment();
}

static void leave_subexpr(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave subexpr\n");

}

static void enter_intexpr(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter intexpr\n");
    increment();
}

static void leave_intexpr(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave intexpr\n");
}

static void enter_doubleexpr(Expression* expr) {
    print_depth();
    fprintf(stderr, "enter doubleexpr\n");
    increment();
}

static void leave_doubleexpr(Expression* expr) {
    decrement();
    print_depth();
    fprintf(stderr, "leave doubleexpr\n");
}

Visitor* create_visitor() {
    visit_expr* enter_list;
    visit_expr* leave_list;
    
    Visitor* visitor = (Visitor*)malloc(sizeof(Visitor));
    enter_list = (visit_expr*)malloc(sizeof(visit_expr) * EXPRESSION_TYPE_COUNT_PLUS_1);
    leave_list = (visit_expr*)malloc(sizeof(visit_expr) * EXPRESSION_TYPE_COUNT_PLUS_1);
    enter_list[MUL_EXPRESSION]    = enter_mulexpr;
    enter_list[ADD_EXPRESSION]    = enter_addexpr;
    enter_list[SUB_EXPRESSION]    = enter_subepr;
    enter_list[INT_EXPRESION]     = enter_intexpr;
    enter_list[DOUBLE_EXPRESSION] = enter_doubleexpr;
    
    leave_list[MUL_EXPRESSION]    = leave_mulexpr;
    leave_list[ADD_EXPRESSION]    = leave_addexpr;
    leave_list[SUB_EXPRESSION]    = leave_subexpr;
    leave_list[INT_EXPRESION]     = leave_intexpr;
    leave_list[DOUBLE_EXPRESSION] = leave_doubleexpr;
    
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


