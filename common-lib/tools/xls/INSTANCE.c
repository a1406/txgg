#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "config_instance.pb-c.h"
#include "freexl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "load_excel.h"

uint8_t outbuf[1024 * 1024 * 5];

CFGINSTANCES result;

void init_result()
{
	cfg__instances__init(&result);
	result.n_instance = max_rows;
	result.instance = malloc(sizeof(void *) * max_rows);
}

ProtobufCMessage *get_data_base(int line)
{
	int i;
	CFGINSTANCE *msg;
	msg = result.instance[line] = malloc(sizeof(CFGINSTANCE));
	cfg__instance__init(msg);

	msg->dropthing_id = malloc(sizeof(int32_t) * 10);
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

	size = cfg__instances__pack(&result, outbuf);

	CFGINSTANCES *check = cfg__instances__unpack(NULL, size, outbuf);	
	
	int fd = open("instance.output", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
	return (0);
}

