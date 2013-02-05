#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>

typedef int (*func_execute_script)(const uint8_t type1,
                   const uint32_t param1,
                   const uint8_t type2,
                   const uint32_t param2,
                   const uint8_t action);

struct script_interface
{
	func_execute_script execute_script;
};

struct script_callback
{
};

typedef struct script_interface* (*func_load_script_so)(void* buff, const int mode, struct script_callback* callback);

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
	struct script_callback script_callback;
	struct script_interface *script_interface = (*load_script)(buff, 0, &script_callback);
	if(!script_interface)
	{
		return -4;
	}
	script_interface->execute_script(1, 2, 3, 4, 5);
	return (0);
}
