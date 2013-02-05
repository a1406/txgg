#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "config_drop.pb-c.h"
#include "freexl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "load_excel.h"

uint8_t outbuf[1024 * 1024 * 5];

CFGDROPS result;

void init_result()
{
	cfg__drops__init(&result);
	result.n_drop = max_rows;
	result.drop = malloc(sizeof(void *) * max_rows);
}

ProtobufCMessage *get_data_base(int line)
{
	int i;
	CFGDROP *msg;
	msg = result.drop[line] = malloc(sizeof(CFGDROP));
	cfg__drop__init(msg);
	msg->item = malloc(sizeof(void *) * 30);
	for (i = 0; i < 30; ++i)
	{
		msg->item[i] = malloc(sizeof(DropItem));
		drop_item__init(msg->item[i]);
	}
//	msg->instance_id = malloc(sizeof(int32_t) * 20);
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

	size = cfg__drops__pack(&result, outbuf);

	CFGDROPS *check = cfg__drops__unpack(NULL, size, outbuf);	
	
	int fd = open("drop.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
	return (0);
}

