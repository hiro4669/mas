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
typedef struct Visitor_tag Visitor;


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
    MINUS_EXPRESSION,
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
    ArgumentList *argument;
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
        Expression             *minus_expression;
        FunctionCallExpression function_call_expression;
    } u;    
};

typedef enum {
    EXPRESSION_STATEMENT = 1,
    GLOBAL_STATEMENT,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    RETURN_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    STATEMENT_TYPE_COUNT_PLUS_1            
} StatementType;

typedef struct Statement_tag Statement;


typedef struct StatementList_tag {
    Statement* statement;
    struct StatementList_tag* next;    
} StatementList;


struct Statement_tag {
    StatementType     type;
    int               line_number;
    union {
        Expression    *expression_s;        
    }u;
};

struct MAS_Interpreter_tag {
    int line_number;
    Expression *expression; // temporary
    MEM_Storage ast_storage;
    Statement  *stmt;       // temporary
    StatementList *stmt_list;
};

/* scanner.c */
int yylex();

/* mas.y */
int yyerror(char const *str);

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
Expression* mas_create_string_expression(char* str);
Expression* mas_create_null_expression();
Expression* mas_create_boolean_expression(MAS_Boolean v);
Expression* mas_create_identifier_expression(char* identifier);
Expression* mas_create_functioncall_expression(char* identifier, ArgumentList* argument);
Expression* mas_create_minus_expression(Expression* operand);
Expression* mas_create_assignment_expression(char* identifier, Expression* operand);

ArgumentList* mas_create_argument_list(Expression* expr);
ArgumentList* mas_chain_argument(ArgumentList* argument, Expression* expr);

Statement* mas_create_expression_statement(Expression* expr);
StatementList* mas_create_statement_list(Statement* stmt);
StatementList* mas_chain_statement_list(StatementList *stmt_list, Statement* stmt);


/* string.c */
void mas_open_string_literal(void);
void mas_add_string_literal(int letter);
void mas_reset_string_literal();
char* mas_close_string_literal();
char* mas_create_identifier(char* str);

/* visitor.c*/
Visitor* create_visitor();

/* traversor.c */
void traverse_expr(Expression* expr, Visitor* visitor);
void traverse_stmt(Statement* stmt,  Visitor* visitor);


#endif /* MAS_H */

