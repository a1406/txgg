#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "../include/script_define.h"

static uint8_t get_type_id(char* type)
{
	assert(type);
	uint8_t i;
	for(i = 0; i < MAX_FUNCTION_TYPE; ++i) {
		if(0 == strcmp(type, type_name[i]))
		{
			return i;
		}
	}

	return (uint8_t)-1;
}

static uint8_t get_action_id(char* action)
{
	assert(action);
	uint8_t i;
	for(i = 0; i < MAX_FUNCTION_ACTION; ++i) {
		if(0 == strcmp(action, action_name[i])) {
			return i;
		}
	}

	return (uint8_t)-1;
}

char *get_type_name(uint8_t type)
{
	assert(type < MAX_FUNCTION_TYPE);
	return type_name[type];
}

char *get_action_name(uint8_t action)
{
	assert(action < MAX_FUNCTION_ACTION);
	return action_name[action];	
}

char *get_next_token(char *data, char seperator)
{
	char *ret = NULL;
	ret = strchr(data, seperator);
	return (ret);
}

int parse_file_name(char *name, uint8_t *type, uint32_t *param_type)
{
	assert(name);
	assert(type);
	assert(param_type);

	char seperator = '_';
	char *next;

	next = get_next_token(name, seperator);
	if (!next)
		return (-1);
	*type = get_type_id(name);
	if (*type == (uint8_t)-1)
		return (-2);
	name = next + 1;
	
	next = get_next_token(name, seperator);
	if (!next)
		return (-10);
	*param_type = (strtoul(name, NULL, 10) & 0xffffffff);

	return 0;
}

int parse_function_name(char *name, uint8_t *action, uint32_t *param_action)
{
	assert(name);
	assert(action);
	assert(param_action);

	char seperator = '_';
	char *next;

	next = get_next_token(name, seperator);
	if (!next)
		return (-1);
	*action = get_action_id(name);
	if (*action == (uint8_t)-1)
		return (-2);
	name = next + 1;
	
	next = get_next_token(name, seperator);
	if (!next)
		return (-10);
	*param_action = (strtoul(name, NULL, 10) & 0xffffffff);

	return 0;

}


int generate_register_sentence(char* sentence, const uint8_t type, const uint32_t param_type, const uint8_t action,
                                                  const uint32_t param_action)
{
	sprintf(sentence, "register_function(%u %u %u %u %s_%u_%s_%u);", type, param_type, action, param_action,
		get_type_name(type), param_type, get_action_name(action), param_action);
	return 0;
}

