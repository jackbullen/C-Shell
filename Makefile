CC = gcc
CFLAGS = -Wall
LDFLAGS = -lreadline -lpthread

SRCS = ./src/main.c ./src/bg_processes.c
OBJS = $(patsubst ./src/%.c, ./obj/%.o, $(SRCS))

TARGET = ./bin/myshell
INF = ./inf
ARGS = ./args

all: $(TARGET)

tests: $(INF) $(ARGS)

$(TARGET): $(OBJS) | bin
	$(CC) -o $@ $^ $(LDFLAGS)

./obj/%.o: ./src/%.c | ./obj
	$(CC) $(CFLAGS) -c $< -o $@

./obj ./bin:
	mkdir -p $@

$(INF): ./test/inf.c
	$(CC) $(CFLAGS) -o $@ $<

$(ARGS): ./test/args.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET) $(INF) $(ARGS) ./obj/*.o
