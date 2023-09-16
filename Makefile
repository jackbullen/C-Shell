CC = gcc

TARGET = myshell

all: $(TARGET)

$(TARGET): main.c
	$(CC) -o $(TARGET) main.c -lreadline

clean:
	rm $(TARGET)
