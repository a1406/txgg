TARGET = game_srv

CONFIG_OBJS = $(wildcard ../tools/config/*.pb-c.o)
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CC = gcc
INCLUDES = -I../../log4c-1.2.1/src  -I../../libevent/include/  -I../../libevent/ -I../lib  -I../common_include -I../proto -I../../protobuf-c/src/ -I../tools/config/  -I../other_include/ -I../other_include/libevent/include -I../../proto -I../script/include
LIBPATH = -L../../libevent/.libs/ -L../../log4c-1.2.1/src/log4c/.libs  -L../lib -L../../protobuf-c/src/.libs
CFLAGS = -g -O0 -Wall -Werror -DDEBUG -DCALC_NET_MSG $(INCLUDES)
LDFLAGS = $(LIBPATH) -export-dynamic -levent -levent_core -levent_extra -llog4c -lrt -lgame -lprotobuf-c -ldl

$(TARGET) : $(OBJS) $(CONFIG_OBJS)  ../../proto/cs.pb-c.o  ../common_include/db_role_info.pb-c.o
	$(CC) $^ -o $@  $(LDFLAGS)

%.o : %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o $(TARGET)

.PHONY:clean