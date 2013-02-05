#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include "game_srv.h"
#include "script.h"

ROLE *cur_role = NULL;
struct script_interface *sc_interface;
static struct script_callback script_callback;
static void *handle;
struct script_interface *load_game_script()
{
	char* pcError;
	
	handle = dlopen("./script.so", RTLD_NOW);
	pcError = dlerror();
	if(pcError)
	{
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s dlopen failed[%s]", __FUNCTION__, pcError);				
		return NULL;
	}

	if (!handle) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s dlopen failed[%d]", __FUNCTION__, errno);
		return NULL;
	}
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s dlopen success", __FUNCTION__);
	
	func_load_script_so load_script;
	load_script = (func_load_script_so)dlsym(handle, "load_script_so");

	pcError = dlerror();
	if(pcError)
	{
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s dlsym failed[%s]", __FUNCTION__, pcError);		
		dlclose(handle);
		return NULL;
	}

	char buff[10240];
	sc_interface = (*load_script)(buff, 0, &script_callback);
	if(!sc_interface)
	{
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s get script interface failed[%d]", __FUNCTION__, errno);		
		dlclose(handle);		
		return NULL;
	}
	return (sc_interface);
}

void close_game_script()
{
	dlclose(handle);
}

void trigger_script_event(ROLE *role, const uint8_t type,
                   const uint32_t param_type,
                   const uint8_t action,
				   const uint32_t param_action)
{
	ROLE *tmp_role = cur_role;
	cur_role = role;
	sc_interface->execute_script(type, param_type, action, param_action);
	cur_role = tmp_role;
}
