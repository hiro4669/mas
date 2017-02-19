/* 
 * File:   mas.h
 * Author: hiroaki
 *
 * Created on February 1, 2017, 2:18 PM
 */

#ifndef _MAS_H_
#define _MAS_H_

#include "./memory/MEM.h"


typedef struct LocalInfo_tag LocalInfo;
typedef struct Expression_tag Expression;
typedef struct MAS_Interpreter_tag MAS_Interpreter;


typedef enum {
    BOOLEAN_EXPRESSION = 1,
    INT_EXPRESION,
    DOUBLE_EXPRESSION,
    STRING_EXPRESSION,
    IDENTIFIER_EXPRESSION,
    ASSIGN_EXPRESSION,
    ADD_EXPRESSION,
    SUB_EXPRESSION,
    MUL_EXPRESSION,
    DIV_EXPRESSION,
    MOD_EXPRESSION,
    EQ_EXPRESSION,
    NE_EXPRESSION,
    GT_EXPRESSION,
    GE_EXPRESSION,
    LT_EXPRESSION,
    LE_EXPRESSION,
    LOGICAL_AND_EXPRESSION,
    LOGICAL_OR_EXPRESSION,
    FUNCTION_CALL_EXPRESSION,
    NULL_EXPRESSION,
    EXPRESSION_TYPE_COUNT_PLUS_1
} ExpressionType;

typedef struct ArgumentList_tag {
    Expression *expression;
    struct ArgumentList_tag *next;    
} ArgumentList;

typedef enum {
    MAS_FALSE = 0,
    MAS_TRUE    
} MAS_Boolean;

typedef struct {
    Expression *left;
    Expression *right;    
} BinaryExpression;

typedef struct {
    char         *variable;
    Expression   *operand;
} AssignExpression;

typedef struct {
    char       *identifier;    
} FunctionCallExpression;

struct Expression_tag {
    ExpressionType type;    
    union {
        MAS_Boolean            boolean_value;
        int                    int_value;
        double                 double_value;
        char                   *string_value;
        char                   *identifier;        
        AssignExpression       assign_expression;
        BinaryExpression       binary_expression; 
        FunctionCallExpression *function_call_expression;
    } u;    
};

struct MAS_Interpreter_tag {
    int line_number;
    Expression *expression;
    MEM_Storage ast_storage;    
};

/*  util.c */
void mas_set_localinfo(LocalInfo *l_info);
LocalInfo *mas_get_localinfo();
void mas_init_localinfo();
void mas_delete_localinfo();
void* mas_malloc(MEM_Storage storage, size_t size);

/* interface.c */
MAS_Interpreter* mas_create_interpreter();
MAS_Interpreter* mas_get_interpreter();
void mas_delete_interpreter();

/* ast.c */
Expression* mas_create_binary_expression(ExpressionType type, Expression* left, Expression* right);
Expression* mas_create_int_expression(int i);
Expression* mas_create_double_expression(double i);

#endif /* MAS_H */

