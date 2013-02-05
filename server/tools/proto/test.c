#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "AMessage.pb-c.h"

void unpack()
{
	char buf[256];
	AMessage *msg;
	size_t size;
	size = read(STDIN_FILENO, buf, 100);
	msg = amessage__unpack(NULL, size, buf);	
}

void bmessage()
{
	BMessage msg = BMESSAGE__INIT;
	Ttt ttt = TTT__INIT;
	msg.n_msg = 3;
	msg.msg = (Ttt **)malloc(sizeof(int) * 3);
	msg.msg[0] = &ttt;
	msg.msg[1] = &ttt;
	msg.msg[2] = &ttt;

	unsigned char *packed;

	size_t size = bmessage__get_packed_size(&msg);
	packed = (unsigned char *)malloc(size);
	bmessage__pack(&msg, packed);
}

int main(int argc, char *argv[])
{
//	bmessage();
//	unpack();
//	return (0);
	
	AMessage msg = AMESSAGE__INIT;

	msg.a = TYPES__CHAR_TYPE;
	msg.b = "tangpeilei";
	msg.n_c = 3;
	msg.c = (int *)malloc(3 * sizeof(int));
	msg.c[0] = 1;
	msg.c[1] = 2;
	msg.c[2] = 3;

	size_t size;
	unsigned char *packed;

	size = amessage__get_packed_size(&msg);
	packed = (unsigned char *)malloc(size);
	amessage__pack(&msg, packed);

	write(STDOUT_FILENO, packed, size);
/*
	AMessage *msg2;
	unsigned char buf[1024];
	
	msg2 = amessage__unpack(NULL, size, packed);
*/	
	return (0);
}

