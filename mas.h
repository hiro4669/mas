/* 
 * File:   mas.h
 * Author: hiroaki
 *
 * Created on February 1, 2017, 2:18 PM
 */

#ifndef _MAS_H_
#define _MAS_H_

#include <stdio.h>
#include "MAS_dev.h"
#include "./memory/MEM.h"


typedef struct LocalInfo_tag LocalInfo;
typedef struct Expression_tag Expression;
typedef struct Visitor_tag Visitor;


/***********************/
/* Definition of Error */
/***********************/
typedef enum {
    PARSE_ERR = 1,
    CHARACTER_INVALID_ERR,
    FUNCTION_MULTIPLE_DEFINE_ERR,
    COMPILE_ERROR_COUNT_PLUS_1
} CompileError;

typedef enum {
    VARIABLE_NOT_FOUND_ERR = 1,
    FUNCTION_NOT_FOUND_ERR,
    ARGUMENT_TOO_MANY_ERR,
    ARGUMENT_TOO_FEW_ERR,
    NOT_BOOLEAN_TYPE_ERR,
    MINUS_OPERAND_TYPE_ERR,
    BAD_OPERAND_TYPE_ERR,
    NOT_BOOLEAN_OPERATOR_ERR,
    FOPEN_ARGUMENT_TYPE_ERR,
    FCLOSE_ARGUMENT_TYPE_ERR,
    FGETS_ARGUMENT_TYPE_ERR,
    FPUTS_ARGUMENT_TYPE_ERR,
    NOT_NULL_OPERATOR_ERR,
    DIVISION_BY_ZERO_ERR,
    GLOBAL_VARIABLE_NOT_FOUND_ERR,
    GLOBAL_STATEMENT_IN_TOPLEVEL_ERR,
    BAD_OPERATOR_FOR_STRING_ERR,
    RUNTIME_ERROR_COUNT_PLUS_1
} RuntimeError;

typedef enum {
    INT_MESSAGE_ARGUMENT = 1,
    DOUBLE_MESSAGE_ARGUMENT,
    STRING_MESSAGE_ARGUMENT,
    CHARACTER_MESSAGE_ARGUMENT,
    POINTER_MESSAGE_ARGUMENT,
    MESSAGE_ARGUMENT_END
} MessageArgumentType;



typedef struct {
    char *format;
} MessageFormat;

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


/********************************/
/*   Definition of Expression   */
/********************************/
typedef struct ArgumentList_tag {
    Expression *expression;
    struct ArgumentList_tag *next;    
} ArgumentList;


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
    int line_number;
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

/********************************/
/*   Definition of Statement   */
/********************************/
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

typedef struct IdentifierList_tag {
    char* identifier;
    struct IdentifierList_tag* next;
} IdentifierList;

typedef struct GlobalStatement_tag {
    IdentifierList *identifier_list;
} GlobalStatement;

typedef struct ReturnStatement_tag {
    Expression* return_value;
} ReturnStatement;

typedef struct Block_tag {
    StatementList* stmt_list;
} Block;

typedef struct WhileStatement_tag {
    Expression* condexpr;
    Block* block;
} WhileStatement;

typedef struct ForStatement_tag {
    Expression* bexpr; // begin expr
    Expression* cexpr; // condition expr
    Expression* iexpr; // iteration expr
    Block*      block;
} ForStatement;

typedef struct Elsif_tag {
    Expression* cexpr;
    Block*      block;
    struct Elsif_tag* next;
} Elsif;

typedef struct IfStatement_tag {
    Expression* condexpr;
    Block*      thenblock;
    Elsif*      elsif;
    Block*      elseblock;
} IfStatement;



struct Statement_tag {
    StatementType     type;
    int               line_number;
    union {
        Expression      *expression_s;
        GlobalStatement global_s;
        ReturnStatement return_s;
        WhileStatement  while_s;
        ForStatement    for_s;
        IfStatement     if_s; 
    }u;
};

typedef enum {
    MAS_FUNCTION = 1,
    NATIVE_FUNCTION,
    FUNCTION_TYPE_PLUS1
} FunctionDefinitionType;

typedef struct ParameterList_tag {
    char* name;
    struct ParameterList_tag *next;
} ParameterList;

typedef struct FunctionDefinition_tag {
    char* name; // function name
    FunctionDefinitionType type;
    union {
        struct {
            ParameterList* param;
            Block*         block;
        } mas_f;
        struct {
            MAS_NativeFunctionProc n_func;
        } native_f;
    }u;
    struct FunctionDefinition_tag* next;
} FunctionDefinition;


/* For Execution */
typedef struct Variable_tag {
    char* name;
    MAS_Value value;
    struct Variable_tag* next;    
} Variable;

typedef enum {
    NORMAL_STATEMENT_RESULT = 1,
    RETURN_STATEMENT_RESULT,
    BREAK_STATEMENT_RESULT,
    CONTINUE_STATEMENT_RESULT,
    STATEMENT_RESULT_TYPE_COUNT_PLUS_1
} StatementResultType;

typedef struct {
    StatementResultType type;
    union {
        MAS_Value return_value;
    } u;
} StatementResult;

typedef struct GlobalVariableRef_tag {
    Variable* variable;
    struct GlobalVariableRef_tag *next;
} GlobalVariableRef;

typedef struct LocalEnvironment_tag {
    Variable* variable;
    GlobalVariableRef* global_variable;
} LocalEnvironment;


#define LINE_BUF_SIZE (1024)

/* Definition of Interpreter*/

struct MAS_Interpreter_tag {
//    int line_number;
    Expression *expression; // temporary    
    MEM_Storage ast_storage;
    MEM_Storage execution_storage;
    Variable* variable;
    Statement  *stmt;       // temporary
    StatementList *stmt_list;
    FunctionDefinition *func_list;
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
FunctionDefinition* mas_search_function(const char* name);
Variable* MAS_search_global_variable(MAS_Interpreter* interp, char* identifier);
void MAS_add_global_variable(MAS_Interpreter* interp, char* identifier, MAS_Value *v);
Variable* MAS_search_local_variable(LocalEnvironment* env, char* identifier);
void MAS_add_local_variable(LocalEnvironment* env, char* identifier, MAS_Value *v);
void mas_show_all_global_variable(MAS_Interpreter* interp); // for debug

/* interface.c */
MAS_Interpreter* mas_create_interpreter();
MAS_Interpreter* mas_get_interpreter();
void MAS_compile(MAS_Interpreter* interp, FILE* fp);
void MAS_interpret(MAS_Interpreter* interp);
void mas_traverse_test();
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
Statement* mas_create_global_statement(IdentifierList* identifier_list);
Statement* mas_create_break_statement();
Statement* mas_create_continue_statement();
Statement* mas_create_return_statement(Expression* expr);
Statement* mas_create_while_statement(Expression* condexpr, Block* block);
Statement* mas_create_for_statement(Expression* bexpr, Expression* cexpr, 
        Expression* iexpr, Block* block);
Statement* mas_create_if_statement(Expression* condexpr, Block* thenblock, 
        Elsif* elsif, Block* elseblock);
FunctionDefinition* mas_create_function_definition(char* name, ParameterList* params, Block* block);
FunctionDefinition* mas_chain_function_definition(FunctionDefinition* flist, FunctionDefinition* newf);

StatementList* mas_create_statement_list(Statement* stmt);
StatementList* mas_chain_statement_list(StatementList *stmt_list, Statement* stmt);
IdentifierList* mas_create_identifier_list(char* identifier);
IdentifierList* mas_chain_identifier_list(IdentifierList* id_list, char* identifier);
Block* mas_create_block(StatementList* stmt_list);
Elsif* mas_create_elsif(Expression* cexpr, Block* block);
Elsif* mas_chain_elsif(Elsif* e_list, Elsif* elsif);
ParameterList* mas_create_parameter(char* name);
ParameterList* mas_chain_parameter(ParameterList *plist, char* name);



/* string.c */
void mas_open_string_literal(void);
void mas_add_string_literal(int letter);
void mas_reset_string_literal();
char* mas_close_string_literal();
char* mas_create_identifier(char* str);
char mas_read_previous();
void mas_add_previous(int letter);

/* visitor.c*/
Visitor* create_visitor();

/* traversor.c */
void traverse_expr(Expression* expr, Visitor* visitor);
void traverse_stmt(Statement* stmt,  Visitor* visitor);
void traverse_func(FunctionDefinition* func, Visitor* visitor);

/* error.c */
void mas_compile_error(CompileError id, ...);
void mas_runtime_error(int line_number, RuntimeError id, ...);

/* native.c */
MAS_Value mas_nv_print(MAS_Interpreter* interp, int arg_count, MAS_Value* args);
MAS_Value mas_nv_open(MAS_Interpreter* interp, int arg_count, MAS_Value* args);
MAS_Value mas_nv_close(MAS_Interpreter* interp, int arg_count, MAS_Value* args);
MAS_Value mas_nv_fputs(MAS_Interpreter* interp, int arg_count, MAS_Value* args);
MAS_Value mas_nv_fgets(MAS_Interpreter* interp, int arg_count, MAS_Value* args);

/* simpleexec.c */
StatementResult mas_execute_statementlist(MAS_Interpreter* interp, 
        LocalEnvironment* env, StatementList* stmt_list);

/* simpleeval.c */
MAS_Value mas_eval_expression(MAS_Interpreter* interp, LocalEnvironment* env, 
        Expression* expr);

/* string_pool.c */
MAS_String* mas_literal_to_mas_string(MAS_Interpreter* interp, char* str);
void mas_refer_string(MAS_String* str);
void mas_release_string(MAS_String* str);
MAS_String* mas_create_mas_string(MAS_Interpreter* interp, char* str);


#endif /* MAS_H */

