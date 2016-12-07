#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include "y.tab.h"

#define ISASCII(c) isascii((unsigned char)(c))
#define ISALNUM(c) (ISASCII(c) && isalnum((unsigned char)(c)))
#define SIGN_EXTEND_CHAR(c) ((signed char)(c))
#define is_identchar(c) (SIGN_EXTEND_CHAR(c)!=-1&&(ISALNUM(c) || (c) == '_'))

#define BUFSIZE 1024
FILE *yyin;
uint8_t *yytext = NULL;
int ytp = 0;

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
    printf("real read\n");
    if (buffer == NULL) {
        buffer = (uint8_t*)malloc(BUFSIZE);
    }
    if (yytext == NULL) {
        yytext = (uint8_t*)malloc(BUFSIZE);
    }
    ptr = 0;
    limit = fread(buffer, 1, BUFSIZE, yyin);
    printf("limit = %d\n", (int)limit);
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

static void addtext(char c) {
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
    
retry:
    switch(c = read()) {
        case ' ':
        case '\t': { // skip space
            goto retry;
        }
        case '\n': {
            goto retry;
        }
        case EOF: {
            printf("eof\n");
            return EOF;
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
        default: {
            printf("end of switch: %c\n", c);
            break;
        }
    }
    while (is_identchar(c)) {
        addtext(c);
        c = read();
    }
    
    pushback(c);
    yylval.identifier = (char*)yytext;
    printf("%s\n", yytext);
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