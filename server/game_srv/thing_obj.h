#ifndef _GAMESRV_THING_OBJ_H__
#define _GAMESRV_THING_OBJ_H__
#include <stdint.h>
#include "game_queue.h"
typedef struct thing_obj_struct
{
	OBJ_BASE(thing_obj_struct) base;
	uint64_t guid;  //不是所有物品都有guid
	uint32_t id;    //物品ID
	uint16_t num;   //数量为0的时候删除
} THING_OBJ;

void init_thing_obj_data(THING_OBJ *thing);
int resume_thing_obj_data(THING_OBJ *thing);

#endif
