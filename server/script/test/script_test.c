#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include "../include/script_define.h"

static struct script_callback script_callback;

char *get_cur_player_name()
{
	return "唐培垒";
}

int main(int argc, char *argv[])
{
	void *handle;
	char* pcError;
	
	handle = dlopen("../so/script.so", RTLD_NOW);
	pcError = dlerror();
	if(pcError)
	{
		printf("dlopen failed, %s\n", pcError);
		return -1;
	}

	if (!handle) {
		printf("dlopen fail[%d]\n", errno);
		return (-2);
	}
	printf("dlopen success\n");
	
	func_load_script_so load_script;
	load_script = (func_load_script_so)dlsym(handle, "load_script_so");

	pcError = dlerror();
	if(pcError)
	{
		printf("dlsym failed, %s\n", pcError);

		dlclose(handle);
		return -3;
	}

	char buff[10240];
	struct script_interface *script_interface = (*load_script)(buff, 0, &script_callback);
	if(!script_interface)
	{
		return -4;
	}
	script_interface->execute_script(TYPE_INSTANCE, 300005, MONSTER_DEAD, 30019602);
	return (0);
}
