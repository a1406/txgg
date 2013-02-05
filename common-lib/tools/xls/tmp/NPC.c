#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "NPC.pb-c.h"
#include "freexl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "load_excel.h"

uint8_t outbuf[1024 * 1024 * 5];

NPCS result;

void init_result()
{
	npcs__init(&result);
	result.n_npc = max_rows;
	result.npc = malloc(sizeof(void *) * max_rows);
}

ProtobufCMessage *get_data_base(int line)
{
	NPC *msg;
	msg = result.npc[line] = malloc(sizeof(NPC));
	npc__init(msg);
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

	size = npcs__pack(&result, outbuf);	
	
	int fd = open("npc.output", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
}

