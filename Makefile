CFLAGS = -c -g -DDEBUG
TARGET = mas
CC = /usr/bin/gcc
MAKE = /usr/bin/make

#OBJS = y.tab.o lex.yy.o lextest.o keyword.o
#OBJS = y.tab.o scanner.o lextest.o keyword.o
#OBJS = y.tab.o scanner.o keyword.o yacctest.o
COMMON = y.tab.o keyword.o util.o interface.o ast.o string.o visitor.o traversor.o \
         error.o error_message.o native.o 
#RUN = simpleeval.o simpleexec.o
RUN = eval.o exec.o
HEAP = string_pool.o heap.o
MEMORY = ./memory/memory.o ./memory/storage.o
OBJS = $(COMMON) $(MEMORY) $(RUN) $(HEAP) scanner.o
YTEST = yacctest.o
EXEC = main.o
LTEST = lextest.o

YACC = mas.y

#all2: scanner.c
#	gcc -o scanner scanner.c

all: $(OBJS) $(EXEC)
	$(MAKE) $@ -C memory
	$(CC) -o $(TARGET) $^

variabletest: $(OBJS) variabletest.o
	$(MAKE) all -C memory
	$(CC) -o $@ $^

stringpooltest: $(OBJS) stringpooltest.o
	$(MAKE) all -C memory
	$(CC) -o $@ $^

errortest: $(MEMORY) error.o error_message.o errortestmain.o
	$(MAKE) all -C memory
	$(CC) -o $@ $^


stringtest: $(MEMORY) string.o interface.o util.o stringtest.o
	$(MAKE) all -C memory
	$(CC) -o $@ $^

asttest: $(OBJS) asttest.o 
	$(CC) -o $@ $^

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
	rm -rf *.o *~ y.tab.c lex.yy.c y.output y.tab.h keyword.c ltest stest output.s output.l $(TARGET)

