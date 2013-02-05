#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "scene.pb-c.h"
#include "freexl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "load_excel.h"

uint8_t outbuf[1024 * 1024 * 5];

SCENES result;

void init_result()
{
	scenes__init(&result);
	result.n_scene = max_rows;
	result.scene = malloc(sizeof(void *) * max_rows);
}

ProtobufCMessage *get_data_base(int line)
{
	int i;
	SCENE *msg;
	msg = result.scene[line] = malloc(sizeof(SCENE));
	scene__init(msg);
	msg->msg = malloc(sizeof(void *) * 10);
	for (i = 0; i < 10; ++i)
	{
		msg->msg[i] = malloc(sizeof(TestMsg));
		test_msg__init(msg->msg[i]);
	}
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

	size = scenes__pack(&result, outbuf);	
	
	int fd = open("scene.output", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
}

