CFLAGS = -c -g -DDEBUG
TARGET = test
CC = /usr/bin/gcc
MAKE = /usr/bin/make

#OBJS = y.tab.o lex.yy.o lextest.o keyword.o
#OBJS = y.tab.o scanner.o lextest.o keyword.o
#OBJS = y.tab.o scanner.o keyword.o yacctest.o
COMMON = y.tab.o keyword.o util.o
MEMORY = ./memory/memory.o ./memory/storage.o
OBJS = $(COMMON) $(MEMORY) scanner.o
YTEST = yacctest.o
LTEST = lextest.o

YACC = mas.y

#all2: scanner.c
#	gcc -o scanner scanner.c

all: $(OBJS) $(YTEST)
	$(MAKE) $@ -C memory
	$(CC) -o $(TARGET) $^

scantest: stest ltest
	./stest > output.s
	./ltest > output.l
	diff -Naru output.s output.l

stest: $(COMMON) $(MEMORY) $(LTEST) scanner.o 
	$(CC) -o $@ $^

ltest: $(COMMON) $(MEMORY) $(LTEST) lex.yy.o
	$(CC) -o $@ $^

keyword.c: keyword.key
	gperf -ptT $^ > $@

y.tab.c: $(YACC)
	bison --yacc -dv $^

lex.yy.c: mas.l
	flex $^

.c.o:
	$(CC) $(CFLAGS) $*.c


clean:
	$(MAKE) $@ -C memory
	rm -rf *.o *~ y.tab.c lex.yy.c y.output y.tab.h keyword.c ltest stest output.s output.l

