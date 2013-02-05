TARGET = proto converter

CONFIG_PATH = ../../../config/
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CC = gcc
INCLUDES = -I../../../freexl/headers/ -I../../../protobuf-c/src/ -I../../other_include/ -I../../other_include/freexl/headers/
LIBPATH = -L../../../freexl/src/.libs -L../../../protobuf-c/src/.libs  -L../../lib
CFLAGS = -g -O0  $(INCLUDES)
LDFLAGS = $(LIBPATH)  -lfreexl -lprotobuf-c  -lm

all: $(TARGET)

converter: $(OBJS)
		$(CC) $^ -o $@ $(LDFLAGS) 	

proto:
	../../bin/protoc-c --c_out=. -I$(CONFIG_PATH)  $(CONFIG_PATH)/*.proto


%.o : %.c
	$(CC) -c $< $(CFLAGS)


clean:
	rm -f *.o $(TARGET)

.PHONY:clean
