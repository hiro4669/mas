/* 
 * File:   mas.h
 * Author: hiroaki
 *
 * Created on February 1, 2017, 2:18 PM
 */

#ifndef _MAS_H_
#define _MAS_H_


typedef struct LocalInfo_tag LocalInfo;


typedef struct Expression_tag Expression;


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

/*  util.c */
void mas_set_localinfo(LocalInfo *l_info);
LocalInfo *mas_get_localinfo();
void mas_init_localinfo();
void mas_delete_localinfo();

#endif /* MAS_H */

