CC = /usr/bin/gcc

TARGET = memtest
CFLAGS = -c -g -DDEBUG

OBJS = memtest.o memory.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ ./memtest.c ./memory.c

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -rf $(TARGET) *.o *~
