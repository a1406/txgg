TARGET = script.so

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CC = gcc
INCLUDES = -I../../lib
LIBPATH = 
CFLAGS = -g -O0 -Wall -Werror -DDEBUG $(INCLUDES) -fpic
LDFLAGS = -fpic -shared ../../lib/sortarray.o

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LDFLAGS) 

%.o : %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o $(TARGET)

.PHONY:clean