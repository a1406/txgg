#ifndef _GAME_SRV_GM_H__
#define _GAME_SRV_GM_H__

#include <stdint.h>
#include "role.h"

#define MAX_GAMEMASTER_PARAMETER_NUMBER                  	16
#define MAX_GAMEMASTER_PER_PARAMETER_LENGTH              	64

typedef int (*func_gm_handle)(ROLE *role, int param_num, char param[MAX_GAMEMASTER_PARAMETER_NUMBER][MAX_GAMEMASTER_PER_PARAMETER_LENGTH]);

int register_gm_handle(func_gm_handle handle, char *cmd);
int run_gm_command(ROLE *role, char *cmd);

#endif

