#ifndef _GAME_SRV_FIGHT_H__
#define _GAME_SRV_FIGHT_H__

#include "role.h"
#include "cs.pb-c.h"

ROLE *choose_rand_role(ROLE *role);
FightNotify *test_fight(ROLE *a, uint32_t param);

//计算战斗结果
FightNotify *count_fight_result(ATTRIBUTE *a, ATTRIBUTE *b);
	

#endif
