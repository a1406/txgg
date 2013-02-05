#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "config_quest_main.pb-c.h"
#include "freexl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "load_excel.h"

uint8_t outbuf[1024 * 1024 * 5];

CFGMAINQUESTS result;

void init_result()
{
	cfg__main__quests__init(&result);
	result.n_quest = max_rows;
	result.quest = malloc(sizeof(void *) * max_rows);
}

ProtobufCMessage *get_data_base(int line)
{
	int i;
	CFGMAINQUEST *msg;
	msg = result.quest[line] = malloc(sizeof(CFGMAINQUEST));
	cfg__main__quest__init(msg);

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

	size = cfg__main__quests__pack(&result, outbuf);

	CFGMAINQUESTS *check = cfg__main__quests__unpack(NULL, size, outbuf);	
	
	int fd = open("quest_main.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
	return (0);
}

