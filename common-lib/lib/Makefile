TARGET = game

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CC = gcc
INCLUDES = -I../../log4c-1.2.1/src  -I../../libevent/include/  -I../../libevent/  -I../common_include/
LIBPATH = -L../../libevent/.libs/ -L../../log4c-1.2.1/src/log4c/.libs 
CFLAGS = -g -O0 -Wall -Werror -DDEBUG_MEM_POOL -DDEBUG -DCALC_NET_MSG $(INCLUDES) -fpic
LDFLAGS = $(LIBPATH) -levent -levent_core -levent_extra -llog4c -lrt -fpic

$(TARGET) : $(OBJS)
	ar rcs lib$@.a $^   

test: testrbtree.c
	gcc -g -O0 -o testrbtree testrbtree.c libgame.a

%.o : %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o lib$(TARGET).a

.PHONY:clean