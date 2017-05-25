#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "mas.h"
#include "y.tab.h"
#include "keyword.h"
#include "info.h"

#define ISASCII(c) isascii((unsigned char)(c))
#define ISALNUM(c) (ISASCII(c) && isalnum((unsigned char)(c)))
#define SIGN_EXTEND_CHAR(c) ((signed char)(c))
#define is_identchar(c) (SIGN_EXTEND_CHAR(c)!=-1&&(ISALNUM(c) || (c) == '_'))

extern struct OPE *in_word_set(char*, unsigned int);



#define FALSE 0
#define TRUE  1
#define BUFSIZE 1024
FILE *yyin;
uint8_t *yytext = NULL;
int ytp = 0;
int yt_max = 0;

static uint8_t *buffer = NULL;
static uint32_t ptr;
static uint32_t limit = 0;

typedef enum {
    INITIAL_STATUS,
    IN_INT_PART_STATUS,
    DOT_STATUS,
    IN_FRAC_PART_STATUS
} LexerStatus;

static void real_read() {
//    printf("real read\n");
    if (buffer == NULL) {
        buffer = (uint8_t*)malloc(BUFSIZE);
    }
    if (yytext == NULL) {
        yytext = (uint8_t*)malloc(BUFSIZE);
        yt_max += BUFSIZE;
    }
    ptr = 0;
    limit = fread(buffer, 1, BUFSIZE, yyin);
//    printf("limit = %d\n", (int)limit);
}


uint8_t read() {    
    if (ptr == limit) real_read();
    if (limit == 0) {
        return EOF;                
    }            
    return buffer[ptr++];
}

void done() {
    free(buffer);
    free(yytext);
}

static void pushback(int c) {
    if (c == -1) return;
    if (ptr > 0) --ptr;
}

/* Dynamically extend yytext */
static void addtext(char c) {
    if (ytp == (yt_max - 1)) {
        yt_max += BUFSIZE;
        yytext = (uint8_t*)realloc(yytext, yt_max);
    }    
    yytext[ytp++] = c;
    yytext[ytp] = 0;
}

static void error() {
    fprintf(stderr, "cannot understand character: %s\n", yytext);
    exit(1);
}

int yylex() {    
    char c; 
    ytp = 0;
//    char *v = "ab\"c";
//    fprintf(stderr, "yylex\n");
retry:
    switch(c = read()) {
        case '#': {
            while ((c = read()) != '\n');
            mas_get_localinfo()->line_number++;
            goto retry;
                
        }
        case ' ':
        case '\t': { // skip space
            goto retry;
        }
        case '\n': {
//            mas_get_localinfo()->line_number++;
            mas_get_localinfo()->line_number++;
            goto retry;
        }
        case EOF: {
//            printf("eof\n");
            return EOF;
        }
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': {
            addtext(c);            
            if (c == '0') { // can be double
                c = read();
                if (c != '.' && isdigit(c)) {
                    error();
                } else if (c == '.') { // double
                    addtext(c);
                    while (isdigit(c = read())) {
                        addtext(c);
                    }
                    pushback(c);
                    double d_value;
                    //                sscanf((char*)yytext, "%lf", &yylval.double_value);
                    sscanf((char*)yytext, "%lf", &d_value);
                    Expression* expr = mas_create_double_expression(d_value);
                    yylval.expression = expr;
                    return DOUBLE_LITERAL;    
                    
                } else {
                    pushback(c);
                    int i_value;
                    sscanf((char*)yytext, "%d", &i_value);
                    Expression* expr = mas_create_int_expression(i_value);
                    yylval.expression = expr;
//                    printf("int value = %d\n", yylval.int_value);
                    return INT_LITERAL;
                    
                }                
                
            } else { // int
                while(isdigit(c = read())) {
                    addtext(c);
                }
                if (c == '.') {
                    addtext(c);
                    while(isdigit(c = read())) {
                        addtext(c);
                    }
                    double d_value;
                    pushback(c);
//                    sscanf((char*)yytext, "%lf", &yylval.double_value);
                    sscanf((char*)yytext, "%lf", &d_value);
                    Expression* expr = mas_create_double_expression(d_value);
                    yylval.expression = expr;
                    return DOUBLE_LITERAL;                      
                } else {
                    pushback(c);
                    int i_value;
                    sscanf((char*)yytext, "%d", &i_value);
                    Expression* expr = mas_create_int_expression(i_value);
//                    sscanf((char*)yytext, "%d", &yylval.int_value);
                    yylval.expression = expr;
                    return INT_LITERAL;
                }
            }
            break;
        }            
        case '(': {
            addtext(c);
            return LP;
        }
        case ')': {
            addtext(c);
            return RP;
        }
        case '{': {
            addtext(c);
            return LC;
        }
        case '}': {
            addtext(c);
            return RC;
        }
        case ';': {
            addtext(c);
            return SEMICOLON;
        }
        case ',': {
            addtext(c);
            return COMMA;
        }
        case '&': {
            addtext(c);
            c = read();
            if (c == '&') {
                addtext(c);
                return LOGICAL_AND;
            } else {
                error();
            }            
        }
        case '|': {
            addtext(c);
            c = read();
            if (c == '|') {
                addtext(c);
                return LOGICAL_OR;
            } else {
                error();
            }
        }
        case '=': {
            addtext(c);
            if ((c = read()) == '=') {
                addtext(c);
                return EQ;
            } else {
                pushback(c);
                return ASSIGN;                
            }
        }
        case '!': {
            addtext(c);
            if ((c = read()) == '=') {
                addtext(c);
                return NE;
            } else {
                error();
            }
        }
        case '>': {
            addtext(c);
            if ((c = read()) == '=') {
                addtext(c);
                return GE;
            } else {
                pushback(c);
                return GT;
            }
        }
        case '<': {
            addtext(c);
            if ((c = read()) == '=') {
                addtext(c);
                return LE;
            } else {
                pushback(c);
                return LT;
            }
        }
        case '+': {
            addtext(c);
            return ADD;
        }
        case '-': {
            addtext(c);
            return SUB;
        }
        case '*': {
            addtext(c);
            return MUL;
        }
        case '/': {
            addtext(c);
            return DIV;
        }
        case '%': {
            addtext(c);
            return MOD;
        }
        case '"': {
            mas_open_string_literal();
            int flg = FALSE;
            while(1) {
                c = read();
//                fprintf(stderr, "c = %c\n", c);
                switch (c) {
                    case 'n': {
//                        fprintf(stderr, "match n\n");
//                        if (yytext[ytp - 1] == '\\') {
                        if (mas_read_previous() == '\\') {
//                            fprintf(stderr, "match previous slash\n");
                            if (flg == TRUE) {
                                mas_add_string_literal(c);
//                                addtext(c);
                                flg = FALSE;
                            } else {
//                                --ytp;
//                                mas_add_string_literal('\n');
                                mas_add_previous('\n');
  //                              addtext('\n');                    
                            }
                            continue;
                        }
                    }
                    case 't': {
                        if (mas_read_previous() == '\\') {
//                        if (yytext[ytp - 1] == '\\') {                            
                            if (flg == TRUE) {
                                mas_add_string_literal(c);
//                                addtext(c);
                                flg = FALSE;
                            } else {
                                --ytp;
//                                mas_add_string_literal('\t');
                                mas_add_previous('\t');
//                                addtext('\t');
                            }
                            continue;
                        }
                    }
                    case '"': {
                        if (mas_read_previous() == '\\') {
//                        if (yytext[ytp - 1] == '\\') {
  //                          --ytp;
//                            mas_add_string_literal('"');
                            mas_add_previous('"');
//                            addtext('"');
                            continue;
                        }
                    }
                    case '\\': {
                        if (mas_read_previous() == '\\') {
//                        if (yytext[ytp - 1] == '\\') {
//                            --ytp;
//                            addtext('\\');
//                            fprintf(stderr, "slash herere\n");
//                            mas_add_string_literal('\\');
                            mas_add_previous('\\');
                            flg = TRUE;
                            continue;
                        }
                    }                 
                }                                
                
                if (c == '"') {
//                    yylval.identifier = (char*)yytext;
//                    yylval.identifier = mas_create_identifier((char*)yytext);
//                    yylval.identifier = mas_close_string_literal();
                    
                    /*
                    int p;
                    char* tmp = mas_close_string_literal();
                    fprintf(stderr, "tmp char len = %d\n", (int)strlen(tmp));
                    for (p = 0; p < strlen(tmp); ++p) {
                        fprintf(stderr, "%02x\n", tmp[p]);
                    }
                    Expression* expr = mas_create_string_expression(tmp);                    
                    */
                    Expression* expr = mas_create_string_expression(mas_close_string_literal());

                    yylval.expression = expr;
                    
                    
                    return STRING_LITERAL;
                }
                mas_add_string_literal(c);
  //              addtext(c);
            }
            error();
        }
        default: {
//            printf("end of switch: %c\n", c);
            break;
        }
    }
    while (is_identchar(c)) {
        addtext(c);
        c = read();
    }
    
    pushback(c);
    struct OPE *op = in_word_set((char*)yytext, strlen((char*)yytext));
    if (op != NULL) {
        return op->type;
    }
    //yylval.identifier = (char*)yytext;
    yylval.identifier = mas_create_identifier((char*)yytext);
    
//    printf("%s\n", yytext);
    return IDENTIFIER;
//    exit(1);
    

//    return EOF;
}

/*
int main(void) {
    int i;
    char c;    
    yyin = fopen("rtest.ma", "r");
    if (yyin == NULL) {
        printf("error\n");
        exit(1);
    }
    for (i = 0; i < 3; ++i) {
        c = read();
        printf("char = %c\n", c);
    }
    printf("ptr = %d\n", ptr);
    c = read();
    printf("char = %c\n", c); 
    printf("ptr = %d\n", ptr);    
    pushback(c);
    printf("ptr = %d\n", ptr);       
    c = read();
    printf("char = %c\n", c); 
    c = read();
    printf("char = %c\n", c); 
    printf("ptr = %d\n", ptr);
    c = read();
    printf("char = %d\n", c); 
    
    printf("EOF = %d\n", EOF);
    return 0;
}
*/