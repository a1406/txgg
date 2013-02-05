#ifndef _GAMESRV_SCRIPT_H__
#define _GAMESRV_SCRIPT_H__
#include "script_define.h"
#include "role.h"

extern ROLE *cur_role;
extern struct script_interface *sc_interface;
extern struct script_interface *load_game_script();

void trigger_script_event(ROLE *role, const uint8_t type,
	                      const uint32_t param_type,
	                      const uint8_t action,
                       	  const uint32_t param_action);

#endif
