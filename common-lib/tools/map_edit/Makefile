TARGET = map_edit

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CC = gcc
INCLUDES = -I../../../protobuf-c/src/ -I../../../freexl/headers/
LIBPATH = -L../../../protobuf-c/src/.libs -L../../../freexl/src/.libs
CFLAGS = -g -O0 -Wall -Werror -DDEBUG $(INCLUDES)
LDFLAGS = $(LIBPATH) -lfreexl -lprotobuf-c  -lm 

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@  $(LDFLAGS)

%.o : %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o $(TARGET)

.PHONY:clean