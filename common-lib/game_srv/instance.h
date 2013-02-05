#ifndef _GAMESRV_INSTANCE_H__
#define _GAMESRV_INSTANCE_H__
#include "game_queue.h"
#include "role.h"

typedef struct instance_struct
{
	OBJ_BASE(instance_struct) base;
	uint16_t id;
	uint8_t role_num;
	IDX role[MAX_ROLE_PER_INSTANCE];	
} INSTANCE, *handle_instance;

INSTANCE *new_instance(uint16_t id);
int enter_instance(ROLE *role, INSTANCE *instance);
int leave_instance(ROLE *role, INSTANCE *instance);

void init_instance_data(INSTANCE *instance);
int resume_instance_data(INSTANCE *instance);

#endif


