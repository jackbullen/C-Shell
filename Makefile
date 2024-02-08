CC = gcc
CFLAGS = -I./include -Wall
LDFLAGS = -lreadline -lpthread

SRCS = ./src/main.c ./src/bg_processes.c
OBJS = $(patsubst ./src/%.c, ./obj/%.o, $(SRCS))

TARGET = ./myshell
INF = ./inf
ARGS = ./args

all: $(TARGET)

tests: $(INF) $(ARGS)

$(TARGET): $(OBJS) | ./bin # bin is not used atm but here for future use
	$(CC) -o $@ $^ $(LDFLAGS)

$(INF): ./test/inf.c
	$(CC) $(CFLAGS) -o $@ $<

$(ARGS): ./test/args.c
	$(CC) $(CFLAGS) -o $@ $<

./obj/%.o: ./src/%.c | ./obj
	$(CC) $(CFLAGS) -c $< -o $@

./obj ./bin:
	mkdir -p $@

clean:
	rm -f $(TARGET) $(INF) $(ARGS) ./obj/*.o
