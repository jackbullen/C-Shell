`CC = gcc

CFLAGS = -g -Wall

TARGET = myshell

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c -lreadline

clean:
	rm $(TARGET)
