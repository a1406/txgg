#include <stdint.h>
#include "script.h"

char *sc_get_cur_player_name()
{
	if (!cur_role)
		return NULL;
	return cur_role->one_role[0].attr.name;
}

uint8_t sc_get_cur_player_prof()
{
	if (!cur_role)
		return 0;
	return cur_role->one_role[0].profession;
}

int sc_give_cur_player_drop(uint32_t drop_id)
{
	if (!cur_role)
		return (-1);
	
	return give_role_drop(cur_role, drop_id, NULL);
}
