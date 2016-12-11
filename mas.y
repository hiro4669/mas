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
/*			: expression { printf("good expression\n"); } */
            : definision_or_statement { printf("accept\n"); }
			| translation_unit definision_or_statement { printf("double accept\n"); }
			;

definision_or_statement 
            : statement
            ;
statement   : expression SEMICOLON
			| global_statement
			;
global_statement
			: GLOBAL_T identifier_list SEMICOLON 
			;
identifier_list
			: IDENTIFIER
			| identifier_list COMMA IDENTIFIER
			;
expression  : logical_or_expression
			| IDENTIFIER ASSIGN expression
			;
logical_or_expression
			: logical_and_expression
			| logical_or_expression LOGICAL_OR logical_and_expression
			;
logical_and_expression
			: equality_expression
			| logical_and_expression LOGICAL_AND equality_expression
			;

equality_expression
			: relational_expression
			| equality_expression EQ relational_expression
			| equality_expression NE relational_expression
			;
relational_expression
			: additive_expression
			| relational_expression GT additive_expression
			| relational_expression GE additive_expression
			| relational_expression LT additive_expression
			| relational_expression LE additive_expression
			;
additive_expression
			: multiplicative_expression
			| additive_expression ADD multiplicative_expression
			| additive_expression SUB multiplicative_expression
			;
multiplicative_expression
			: unary_expression
			| multiplicative_expression MUL unary_expression
			| multiplicative_expression DIV unary_expression
			| multiplicative_expression MOD unary_expression
			;
unary_expression
			: primary_expression
			| SUB unary_expression /*{ printf("sub unary\n"); } */
			;
argument_list
            : expression
			| argument_list COMMA expression
			;
primary_expression
			: INT_LITERAL
			| IDENTIFIER LP RP
			| IDENTIFIER LP argument_list RP
			| LP expression RP
			| IDENTIFIER
			/*| INT_LITERAL*/
			| DOUBLE_LITERAL
			| STRING_LITERAL
			| TRUE_T
			| FALSE_T
			| NULL_T
			;


%%
int
yyerror(char const *str)
{
    extern char *yytext;
    fprintf(stderr, "parser error near %s\n", yytext);
    return 0;
}



