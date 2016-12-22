CC = /usr/bin/gcc

TARGET = memtest

all: $(TARGET)

$(TARGET): memtest.c
	$(CC) -o $@ ./memtest.c

clean:
	rm -rf $(TARGET) *.o *~
