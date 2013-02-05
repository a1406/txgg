TARGET = script_test

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CC = gcc
INCLUDES = -I../../log4c-1.2.1/src  -I../../libevent/include/  -I../../libevent/  -I../../lib
LIBPATH =  -L../../lib 
CFLAGS = -g -O0 -Wall -Werror -DDEBUG $(INCLUDES)
LDFLAGS = $(LIBPATH) -lgame 

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@ -ldl $(LDFLAGS)

%.o : %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o $(TARGET)

.PHONY:clean