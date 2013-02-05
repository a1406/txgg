TARGET = db_srv

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CC = gcc
INCLUDES = -I../../log4c-1.2.1/src  -I../../libevent/include/  -I../../libevent/  -I../lib  -I../common_include -I../other_include/ -I../other_include/libevent/include -I../../proto
LIBPATH = -L../../libevent/.libs/ -L../../log4c-1.2.1/src/log4c/.libs  -L../lib -L../lib -L../../protobuf-c/src/.libs
CFLAGS = -g -O0 -Wall -Werror -DDEBUG -DCALC_NET_MSG $(INCLUDES)
LDFLAGS = $(LIBPATH) -levent -levent_core -levent_extra -llog4c -lrt -lgame -lprotobuf-c -lmysqlclient

$(TARGET) : $(OBJS) ../../proto/cs.pb-c.o ../common_include/db_role_info.pb-c.o
	$(CC) $^ -o $@ $(INCLUDES) $(LDFLAGS) 

%.o : %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o $(TARGET)

.PHONY:clean