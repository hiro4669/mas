%{
#include <stdio.h>
#include "mas.h"
#define YYDEBUG 1
%}
%union {
    /*
	int int_value;
	double double_value;
     */
	char *identifier;
    Expression* expression;
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
%token <expression>   INT_LITERAL
%token <expression>   DOUBLE_LITERAL
%token <expression>   STRING_LITERAL

%type <expression> expression 
		logical_and_expression logical_or_expression
		equality_expression relational_expression
		additive_expression multiplicative_expression
		unary_expression primary_expression



%%
translation_unit 
/*			: expression { printf("good expression\n"); } */
            : definision_or_statement { printf("accept\n"); }
			| translation_unit definision_or_statement { printf("double accept\n"); }
			;

definision_or_statement 
            : statement
			| function_definition
            ;

function_definition
			: FUNCTION IDENTIFIER LP parameter_list RP block
			| FUNCTION IDENTIFIER LP                RP block
			;

parameter_list
			: IDENTIFIER
			| parameter_list COMMA IDENTIFIER
			;

statement_list
			: statement
			| statement_list statement
			;

statement   : expression SEMICOLON { printf("expr statement\n"); }
			| global_statement
			| while_statement 
			| return_statement
			| break_statement
			| continue_statement
			| for_statement /*{ printf("accept for "); } */
			| if_statement
			;
global_statement
			: GLOBAL_T identifier_list SEMICOLON 
			;
identifier_list
			: IDENTIFIER
			| identifier_list COMMA IDENTIFIER
			;

expression              : logical_or_expression { 
    printf("expression\n");
    Expression* expr;
    MAS_Interpreter* interp;
    expr = $1;
    interp = mas_get_interpreter();
    interp->expression = expr;
    if (expr) {
        printf("type = %d\n", expr->type);
    } else {
        printf("no expr\n");
    }
}
                        | IDENTIFIER ASSIGN expression  { $$ = NULL; }
			;
logical_or_expression
			: logical_and_expression { $$ = $1; }
			| logical_or_expression LOGICAL_OR logical_and_expression
			;
logical_and_expression
			: equality_expression { printf("logical_and expr\n");  }
			| logical_and_expression LOGICAL_AND equality_expression
			;

equality_expression
			: relational_expression { printf("equality expr\n"); }
			| equality_expression EQ relational_expression
			| equality_expression NE relational_expression
			;
relational_expression
			: additive_expression { printf("relational expr\n"); }
			| relational_expression GT additive_expression
			| relational_expression GE additive_expression
			| relational_expression LT additive_expression
			| relational_expression LE additive_expression
			;
additive_expression
			: multiplicative_expression { printf("additive expr\n"); }
			| additive_expression ADD multiplicative_expression { 
                            printf("additive_expression AND\n");
                            $$ = mas_create_binary_expression(ADD_EXPRESSION, $1, $3);
                        }
			| additive_expression SUB multiplicative_expression {
                            printf("additive_expression SUB\n");
                            $$ = mas_create_binary_expression(SUB_EXPRESSION, $1, $3);
                        }
			;
multiplicative_expression
			: unary_expression { printf("multiplicative expr\n"); }
			| multiplicative_expression MUL unary_expression
			| multiplicative_expression DIV unary_expression
			| multiplicative_expression MOD unary_expression
			;
unary_expression
			: primary_expression   { printf("unary expr\n"); }
			| SUB unary_expression { printf("sub unary\n"); } 
			;
argument_list
                        : expression
			| argument_list COMMA expression
			;
primary_expression
			: IDENTIFIER LP RP { $$ = NULL; }
			| IDENTIFIER LP argument_list RP { $$ = NULL; }
			| LP expression RP { $$ = NULL; }
			| IDENTIFIER       { $$ = mas_create_identifier_expression($1); }
			| INT_LITERAL      
			| DOUBLE_LITERAL
			| STRING_LITERAL
			| TRUE_T           { $$ = mas_create_boolean_expression(MAS_TRUE); }
			| FALSE_T          { $$ = mas_create_boolean_expression(MAS_FALSE); }
			| NULL_T           { $$ = mas_create_null_expression(); }
			;

while_statement
			: WHILE LP expression RP block

block : LC statement_list RC
	  | LC RC
	  ;
expression_opt
			: /* empty */ 
			| expression
			;
return_statement
			: RETURN_T expression_opt SEMICOLON
			;
break_statement
			: BREAK SEMICOLON
			;
continue_statement
			: CONTINUE SEMICOLON
			;

for_statement
			: FOR LP expression_opt SEMICOLON expression_opt SEMICOLON expression_opt RP block
			;

if_statement
			: IF LP expression RP block { printf("if match "); }
			| IF LP expression RP block ELSE block { printf("if match 2 "); }
			| IF LP expression RP block elsif_list { printf("if match 3 "); }
			| IF LP expression RP block elsif_list ELSE block { printf("if match 4 "); }
			;

elsif_list : elsif
		   | elsif_list elsif
		   ;

elsif	   : ELSIF LP expression RP block
		   ;

%%
int
yyerror(char const *str)
{
    extern char *yytext;
    fprintf(stderr, "parser error near %s\n", yytext);
    return 0;
}



