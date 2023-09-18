CC = gcc

TARGET = myshell

all: $(TARGET) inf args

$(TARGET): main.c
	$(CC) -o $(TARGET) main.c -lreadline

inf: tests/inf.c
	gcc tests/inf.c -o tests/inf

args: tests/args.c
	gcc tests/args.c -o tests/args

clean:
	rm $(TARGET)
