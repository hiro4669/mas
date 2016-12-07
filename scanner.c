#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BUFSIZE 3
FILE *yyin;
uint8_t *yytext = NULL;

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
    printf("limit = %d\n", limit);
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

void pushback(int c) {
    if (c == -1) return;
    if (ptr > 0) --ptr;
}

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