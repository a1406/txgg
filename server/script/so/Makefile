TARGET = script.so

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CC = gcc
INCLUDES = -I../../lib -I../include
LIBPATH = -L../../lib
CFLAGS = -g -O0 -Wall -Werror -DDEBUG $(INCLUDES) -fpic
LDFLAGS = $(LIBPATH) -fpic -shared -lgame

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LDFLAGS) 
	cp script.so ../../game_srv/

%.o : %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o $(TARGET)

.PHONY:clean