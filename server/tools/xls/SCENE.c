#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "config_scene.pb-c.h"
#include "freexl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "load_excel.h"

uint8_t outbuf[1024 * 1024 * 5];

CFGSCENES result;

void init_result()
{
	cfg__scenes__init(&result);
	result.n_scene = max_rows;
	result.scene = malloc(sizeof(void *) * max_rows);
}

ProtobufCMessage *get_data_base(int line)
{
	int i;
	CFGSCENE *msg;
	msg = result.scene[line] = malloc(sizeof(CFGSCENE));
	cfg__scene__init(msg);
	msg->npc_info = malloc(sizeof(void *) * 50);
	for (i = 0; i < 50; ++i)
	{
		msg->npc_info[i] = malloc(sizeof(NpcInfo));
		npc_info__init(msg->npc_info[i]);
	}
	msg->instance_id = malloc(sizeof(int32_t) * 50);
	return &msg->base;
}

int main(int argc, char *argv[])
{
	size_t size;
	if (argc < 2) {
		printf("please input xls file name\n");
		return (0);
	}
	if (load_excel(argv[1]) != 0)
	{
		printf("load excel fail\n");
		return (0);		
	}
	init_result();	
	size = generate_pb_data(get_data_base);

	size = cfg__scenes__pack(&result, outbuf);

	CFGSCENES *check = cfg__scenes__unpack(NULL, size, outbuf);	
	
	int fd = open("scene.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
	return (0);
}

