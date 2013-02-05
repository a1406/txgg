TARGET = flashsrvd

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CC = gcc
INCLUDES = -I../../log4c-1.2.1/src  -I../../libevent/include/  -I../../libevent/  -I../lib 
LIBPATH = -L../../libevent/.libs/ -L../../log4c-1.2.1/src/log4c/.libs  -L../lib
CFLAGS = -g -O0 -Wall -Werror -DDEBUG $(INCLUDES)
LDFLAGS = $(LIBPATH) -levent -levent_core -levent_extra -llog4c -lrt -lgame

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LDFLAGS) 

%.o : %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o $(TARGET)

.PHONY:clean