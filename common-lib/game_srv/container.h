#ifndef _GAMESRV_CONTAINER_H__
#define _GAMESRV_CONTAINER_H__
#include "game_queue.h"
#include <stdint.h>
#include "idx.h"
#include "thing_obj.h"
#include "game_define.h"

typedef struct container_struct
{
	OBJ_BASE(container_struct) base;
	uint8_t max_thing_num;                         //最大可用格子数
	IDX things[MAX_THING_OBJ_PER_CONTAINER];
} CONTAINER;

void init_container_data(CONTAINER *container);
int resume_container_data(CONTAINER *container);

int add_thing_obj(CONTAINER *container, THING_OBJ *thing);
int delete_thing_obj(CONTAINER *container, THING_OBJ *thing);
int count_thing_num(CONTAINER *container, uint32_t id);
int delete_thing_byid(CONTAINER *container, uint32_t id, uint16_t num);

#endif
