%{
#include <stdio.h>
#define YYDEBUG 1
%}
%union {
	int int_value;
	double double_value;
	char *identifier;
}

%token FUNCTION
%token IF
%token ELSE
%token ELSIF
%token WHILE
%token FOR
%token RETURN_T
%token BREAK
%token CONTINUE
%token NULL_T
%token TRUE_T
%token FALSE_T
%token GLOBAL_T
%token LP
%token RP
%token LC
%token RC
%token SEMICOLON
%token COMMA
%token LOGICAL_AND
%token LOGICAL_OR
%token ASSIGN
%token EQ
%token NE
%token GT
%token GE
%token LT
%token LE
%token ADD
%token SUB
%token MUL
%token DIV
%token MOD

%token <identifier>   IDENTIFIER
%token <int_value>    INT_LITERAL
%token <double_value> DOUBLE_LITERAL
%token STRING_LITERAL


%%
translation_unit 
			: expression { printf("good expression\n"); }
			;
expression              : logical_or_expression
			;
logical_or_expression
			: logical_and_expression
			;
logical_and_expression
			: equality_expression
			;

equality_expression
			: relational_expression
			;
relational_expression
			: additive_expression
			;
additive_expression
			: multiplicative_expression
			;
multiplicative_expression
			: unary_expression
			;
unary_expression
			: primary_expression
			;
primary_expression
			: INT_LITERAL
			;


%%
int
yyerror(char const *str)
{
    extern char *yytext;
    fprintf(stderr, "parser error near %s\n", yytext);
    return 0;
}



