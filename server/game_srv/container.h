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

struct things
{
	uint32_t id;     //物品ID
	uint32_t num;    //物品数目
	uint8_t pos;     //成功后返回实际放置的位置
};
struct thing_list
{
	uint8_t num;
	struct things things[MAX_THINGLIST_NUM];
};

void init_container_data(CONTAINER *container);
int resume_container_data(CONTAINER *container);
int count_thing_num(CONTAINER *container, uint32_t id);

//返回变化的pos，小于0表示失败
int add_thing_list(CONTAINER *container, struct thing_list *list);

int add_thing_bypos(CONTAINER *container, uint8_t pos, uint32_t id, uint16_t num);
int delete_thing_bypos(CONTAINER *container, uint8_t pos, uint16_t num);
int swap_thing_pos(CONTAINER *container, uint8_t pos1, uint8_t pos2);

THING_OBJ *get_thingobj_bypos(CONTAINER *container, uint8_t pos);

#endif
