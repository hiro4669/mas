CFLAGS = -c -g -DDEBUG
TARGET = test
CC = /usr/bin/gcc

#OBJS = y.tab.o lex.yy.o lextest.o keyword.o
OBJS = y.tab.o scanner.o lextest.o keyword.o

#all2: scanner.c
#	gcc -o scanner scanner.c

all: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

keyword.c: keyword.key
	gperf -ptT $^ > $@

y.tab.c: mas.y
	bison --yacc -dv $^

lex.yy.c: mas.l
	flex $^

.c.o:
	$(CC) $(CFLAGS) $*.c


clean:
	rm -rf *.o *~ y.tab.c lex.yy.c y.output y.tab.h keyword.c

