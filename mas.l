%{
#include <stdio.h>
#include <string.h>
#include "mas.h"
#include "y.tab.h"

int yywrap(void)
{
	return 1;
//	return 0;
}

char buf[256];
int pos;

void addchar(char c) {
    buf[pos] = c;
	pos++;
	buf[pos] = '\0';
}
%}

%start COMMENT STRING_LITERAL_STATE

%%

<INITIAL>"function"    return FUNCTION;
<INITIAL>"if"          return IF;
<INITIAL>"else"        return ELSE;
<INITIAL>"elsif"       return ELSIF;
<INITIAL>"while"       return WHILE;
<INITIAL>"for"         return FOR;
<INITIAL>"return"      return RETURN_T;
<INITIAL>"break"       return BREAK;
<INITIAL>"continue"    return CONTINUE;
<INITIAL>"null"        return NULL_T;
<INITIAL>"true"        return TRUE_T;
<INITIAL>"false"       return FALSE_T;
<INITIAL>"global"      return GLOBAL_T;
<INITIAL>"("           return LP;
<INITIAL>")"           return RP;
<INITIAL>"{"           return LC;
<INITIAL>"}"           return RC;
<INITIAL>";"           return SEMICOLON;
<INITIAL>","           return COMMA;
<INITIAL>"&&"          return LOGICAL_AND;
<INITIAL>"||"          return LOGICAL_OR;
<INITIAL>"="           return ASSIGN;
<INITIAL>"=="          return EQ;
<INITIAL>"!="          return NE;
<INITIAL>">"           return GT;
<INITIAL>">="          return GE;
<INITIAL>"<"           return LT;
<INITIAL>"<="          return LE;
<INITIAL>"+"           return ADD;
<INITIAL>"-"           return SUB;
<INITIAL>"*"           return MUL;
<INITIAL>"/"           return DIV;
<INITIAL>"%"           return MOD;
<INITIAL><<EOF>>       return EOF;
<INITIAL>[A-Za-z_][A-Za-z_0-9]* {
	sscanf(yytext, "%s", buf);
	yylval.identifier = buf;
	/*sscanf(yytext, "%s", yylval.identifier);*/
	return IDENTIFIER;
}
<INITIAL>([1-9][0-9]*)|"0" {
    int i_value;
    sscanf(yytext, "%d", &i_value); 
    Expression* expr = mas_create_int_expression(i_value);
    yylval.expression = expr;
//    sscanf(yytext, "%d", &(yylval.int_value));     
    return INT_LITERAL;
}
<INITIAL>[0-9]+\.[0-9]+ {
    double d_value;
//    sscanf(yytext, "%lf", &(yylval.double_value));
    sscanf(yytext, "%lf", &d_value);
    Expression* expr = mas_create_double_expression(d_value);
    yylval.expression = expr;
    return DOUBLE_LITERAL;
}
<INITIAL>\" {
    pos = 0;
	BEGIN STRING_LITERAL_STATE;
}
<INITIAL>[ \t] ;
<INITIAL>\n    ;
<INITIAL># {BEGIN COMMENT;}
<INITIAL>. {
	fprintf(stderr, "bad_character\n");
}

<COMMENT>\n {
	BEGIN INITIAL;
}
<COMMENT>.  ;

<STRING_LITERAL_STATE>\" {
//    yylval.identifier = buf;
        Expression* expr = mas_create_string_expression(buf);
yylval.expression = expr;
//    yylval.identifier = buf;
	BEGIN INITIAL;
	return STRING_LITERAL;
}
<STRING_LITERAL_STATE>\n {
	;
}
<STRING_LITERAL_STATE>\\\"  { addchar('"'); }
<STRING_LITERAL_STATE>\\n   { addchar('\n') ;}
<STRING_LITERAL_STATE>\\t   { addchar('\t') ;}
<STRING_LITERAL_STATE>\\\\  { addchar('\\') ;}
<STRING_LITERAL_STATE>.     {addchar(yytext[0]);}

%%


