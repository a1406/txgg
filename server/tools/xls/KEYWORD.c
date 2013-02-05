#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "config_keyword.pb-c.h"
#include "freexl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "load_excel.h"

uint8_t outbuf[1024 * 1024 * 5];

CFGKEYWORDS result;

void init_result()
{
	cfg__keywords__init(&result);
	result.n_keyword = max_rows;
	result.keyword = malloc(sizeof(void *) * max_rows);
}

ProtobufCMessage *get_data_base(int line)
{
	int i;
	CFGKEYWORD *msg;
	msg = result.keyword[line] = malloc(sizeof(CFGKEYWORD));
	cfg__keyword__init(msg);
//	msg->item = malloc(sizeof(void *) * 30);
//	for (i = 0; i < 30; ++i)
//	{
//		msg->item[i] = malloc(sizeof(DropItem));
//		drop_item__init(msg->item[i]);
//	}
//	msg->instance_id = malloc(sizeof(int32_t) * 20);
	return &msg->base;
}

int generate_proto_file()
{
	static char keyword_buf[1024 * 1024 * 3];
	int fd;
	size_t size = 0;
	int i;
	int j;

	size += sprintf(&keyword_buf[size], "enum CFG_KEYWORD_NAME\n{\n");
	
	for (i = 0; i < max_rows; ++i) {
		if (!data[i][1])
			continue;
		size += sprintf(&keyword_buf[size], "	%s = %d;\n", data[i][1], atoi(data[i][2]));
	}
	size += sprintf(&keyword_buf[size], "}\n");	
	
	fd = open("keyword.proto", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, keyword_buf, size) != size) {
		perror("write fail\n");
	}
	close(fd);


	size = 0;
	size += sprintf(&keyword_buf[size], "package com.koala.data.cfg {\n\n	public final class KeywordGroupID {\n\n");
	
	for (i = 0; i < max_rows; ++i) {
		if (!data[i][1])
			continue;
		for (j = 0; j < i; j++) {
			if (strcmp(data[i][0], data[j][0]) == 0)
				break;
		}
		if (j < i)
			continue;
		
		size += sprintf(&keyword_buf[size], "		public static const %s:String = \'%s\';\n", data[i][0], data[i][0]);
	}
	size += sprintf(&keyword_buf[size], "\n	}\n}\n");	
	
	fd = open("KeywordGroupID.as", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, keyword_buf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
	
	return (0);
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

	size = cfg__keywords__pack(&result, outbuf);

	CFGKEYWORDS *check = cfg__keywords__unpack(NULL, size, outbuf);	
	
	int fd = open("keyword.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);

	generate_proto_file();	
	return (0);
}

