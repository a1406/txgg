#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "sortarray.h"
#include "../include/script_define.h"

static int registed_function_num = 0;
static REGISTED_FUNCTION_DATA registed_functions[1024];



int instance_1001010_monster_202020_()
{
	return (0);
}

int instance_1001010_instanceMonsterDead_201020()
{
	return (0);
}

struct script_interface
{
	func_execute_script execute_script;
};

struct script_callback
{
};

int function_comp(const void *a, const void *b)
{
	const REGISTED_FUNCTION_DATA *param_a = a;
	const REGISTED_FUNCTION_DATA *param_b = b;

	if (param_a->param.data[0] > param_b->param.data[0])
		return 1;

	if (param_a->param.data[0] < param_b->param.data[0])
		return -1;

	if (param_a->param.data[1] > param_b->param.data[1])
		return 1;

	if (param_a->param.data[1] < param_b->param.data[1])
		return -1;

	return (0);
}

int execute_script(const uint8_t type,
                   const uint32_t param_type,
                   const uint8_t action,
				   const uint32_t param_action)
{
	int find, pos;
	REGISTED_FUNCTION_DATA data = {.param = {.param = {.type = type,
													   .param_type = param_type,
													   .action = action,
													   .param_action = param_action}}};
	printf("%s in %s, action is %d\n", __FUNCTION__, __FILE__, action);
	pos = array_bsearch(&data, &registed_functions[0], registed_function_num, sizeof(data), &find, function_comp);
	if (!find)
		return (0);
	assert(pos >= 0 && pos < registed_function_num);
	return registed_functions[pos].func();
}

static struct script_interface interface =
{
	.execute_script = execute_script
};
static struct script_callback script_callback;

struct script_interface* load_script_so(void* buff, const int mode, struct script_callback* callback)
{
	memcpy(&script_callback, callback, sizeof(struct script_callback));
	return &interface;
}



void register_function(
    const uint8_t type,
	const uint32_t param_type,
	const uint8_t action,
	const uint32_t param_action,
	const func_script func)
{
    if (registed_function_num >= (sizeof(registed_functions) / sizeof(registed_functions[0])))
    {
        printf("Too many Script Function!\n");
		return;
    }

	REGISTED_FUNCTION_DATA function_data;
	function_data.param.param.type = type;
	function_data.param.param.param_type = param_type;
	function_data.param.param.action = action;
	function_data.param.param.param_action = param_action;
	function_data.func = func;		
	
    int iRet = array_insert(&function_data, &registed_functions[0], &registed_function_num, sizeof(function_data), 1, function_comp);
    if (iRet < 0)
    {
		printf("array_insert fail\n");
//        printf("RegisterScript Failed: %d:%d:%d:%d:%d\n", iScriptSectionID, iScriptThingID, iScriptMethodID, iContextThingID, iContextThingID);
        return;
    }

//    printf("register_function: %s_%d.cxx: ATM_%s_%d_%s\n",
//        CScriptSymbolParser::GetSectionName(type1), 
//        param1, 
//        CScriptSymbolParser::GetSectionName(type2), 
//        param2, 
//        CScriptSymbolParser::GetMethodName(action));
}
