#all: test.c
#	gcc -g -O0 -c test.c -I../../../freexl/headers/
#	gcc -o test test.o -L../../../freexl/src/.libs -lfreexl -lm

#	/home/jacktang/soft/gitroot/protobuf-c/src/protoc-c  --c_out=. NPC.proto
#	gcc -g -O0 -c NPC.pb-c.c  -I../../../protobuf-c/src/
#	gcc -g -O0 -c NPC.c -I../../../freexl/headers/ -I../../../protobuf-c/src/
#	gcc -g -O0 -c load_excel.c -I../../../freexl/headers/ -I../../../protobuf-c/src/
#	gcc -o NPC NPC.o load_excel.o NPC.pb-c.o -L../../../freexl/src/.libs -L../../../protobuf-c/src/.libs -lfreexl -lm -lprotobuf-c

#clean:
#	rm -rf test NPC *.o


TARGET: proto NPC TRANSPORT SCENE INSTANCE SKILL MAP_EDIT PLAYER DROP

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CC = gcc
INCLUDES = -I../../../freexl/headers/ -I../../../protobuf-c/src/
LIBPATH = -L../../../freexl/src/.libs -L../../../protobuf-c/src/.libs 
CFLAGS = -g -O0  $(INCLUDES)
LDFLAGS = $(LIBPATH)  -lfreexl -lprotobuf-c  -lm

COMMON_OBJS = load_excel.o generate_pb.o

DROP: $(COMMON_OBJS) DROP.o config_drop.pb-c.o
	$(CC) $^ -o $@ $(LDFLAGS) 

PLAYER: $(COMMON_OBJS) PLAYER.o config_player.pb-c.o
	$(CC) $^ -o $@ $(LDFLAGS) 

SKILL: $(COMMON_OBJS) SKILL.o config_skill.pb-c.o
	$(CC) $^ -o $@ $(LDFLAGS) 

NPC: $(COMMON_OBJS) NPC.o config_NPC.pb-c.o
	$(CC) $^ -o $@ $(LDFLAGS) 

TRANSPORT: $(COMMON_OBJS) TRANSPORT.o transport.pb-c.o
	$(CC) $^ -o $@ $(LDFLAGS) 

SCENE: $(COMMON_OBJS) SCENE.o config_scene.pb-c.o
	$(CC) $^ -o $@ $(LDFLAGS) 

INSTANCE: $(COMMON_OBJS) INSTANCE.o config_instance.pb-c.o
	$(CC) $^ -o $@ $(LDFLAGS) 

MAP_EDIT: process_map.o config_instance_map.pb-c.o
	$(CC) $^ -o $@ $(LDFLAGS) 

proto:
	../../../protobuf-c/src/protoc-c  --c_out=. *.proto

%.o : %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o $(TARGET)

.PHONY:clean
