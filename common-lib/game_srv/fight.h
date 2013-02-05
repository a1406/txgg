#ifndef _GAME_SRV_FIGHT_H__
#define _GAME_SRV_FIGHT_H__

#include "role.h"
#include "cs.pb-c.h"

void test_fight();

//计算战斗结果
FightNotify *count_fight_result(ROLE *a, ROLE *b);
	

#endif
