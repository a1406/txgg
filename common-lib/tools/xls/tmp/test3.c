#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "scene.pb-c.h"
#include "freexl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(int argc, char *argv[])
{

	SCENE scene;
	TestMsg msg;

	test_msg__init(&msg);
	scene__init(&scene);
	scene.n_msg = 3;
	scene.msg = malloc(sizeof(void*) * 3);
	scene.msg[0] = &msg;
	scene.msg[1] = &msg;
	scene.msg[2] = &msg;	

	msg.pos_x = 10;
	msg.pos_y = 20;
	
	return (0);
}
