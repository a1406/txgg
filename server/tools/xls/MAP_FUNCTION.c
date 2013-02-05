#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "config_map_function.pb-c.h"
#include "freexl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "load_excel.h"

uint8_t outbuf[1024 * 1024 * 5];

CFGMAPFUNCTIONS result;

void init_result()
{
	cfg__map__functions__init(&result);
	result.n_map_function = max_rows;
	result.map_function = malloc(sizeof(void *) * max_rows);
}

ProtobufCMessage *get_data_base(int line)
{
	int i;
	CFGMAPFUNCTION *msg;
	msg = result.map_function[line] = malloc(sizeof(CFGMAPFUNCTION));
	cfg__map__function__init(msg);
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

	size = cfg__map__functions__pack(&result, outbuf);

	CFGMAPFUNCTIONS *check = cfg__map__functions__unpack(NULL, size, outbuf);	
	
	int fd = open("mapfunction.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
	return (0);
}

