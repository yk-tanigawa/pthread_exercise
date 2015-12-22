CC = gcc
LD = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpthread -lm
SRCS := $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.dep)
EXEC = $(SRCS:.c=)
RM = rm -f

all: $(EXEC)

test: test.o
	$(LD) $(LDFLAGS) -o $@ $^

test.o: pthread.h


clean:
	$(RM) $(OBJS) $(EXEC) *~

.PHONY:
	all clean
