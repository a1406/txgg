#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "config_player.pb-c.h"
#include "freexl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "load_excel.h"

uint8_t outbuf[1024 * 1024 * 5];

CFGPLAYERS result;

void init_result()
{
	cfg__players__init(&result);
	result.n_player = max_rows;
	result.player = malloc(sizeof(void *) * max_rows);
}

ProtobufCMessage *get_data_base(int line)
{
	int i;
	CFGPLAYER *msg;
	msg = result.player[line] = malloc(sizeof(CFGPLAYER));
	cfg__player__init(msg);
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

	size = cfg__players__pack(&result, outbuf);	
	
	int fd = open("player.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
}

