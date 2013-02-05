#ifndef _GAMESRV_INSTANCE_H__
#define _GAMESRV_INSTANCE_H__
#include "game_queue.h"
#include "role.h"

struct instance_node
{
	uint16_t id;
	int16_t seq_id;	
	int32_t action;
	int16_t up_pos;
	int16_t left_pos;
	int16_t down_pos;
	int16_t right_pos;
};

typedef struct instance_struct
{
	OBJ_BASE(instance_struct) base;
	uint16_t id;
	uint8_t active_role;
	uint8_t role_num;
	IDX role[MAX_ROLE_PER_INSTANCE];
	uint8_t node_num;
	struct instance_node node[MAX_NODE_PER_INSTANCE];
} INSTANCE, *handle_instance;

INSTANCE *new_instance(uint16_t id);
int enter_instance(ROLE *role, INSTANCE *instance);
int leave_instance(ROLE *role, INSTANCE *instance);

void init_instance_data(INSTANCE *instance);
int resume_instance_data(INSTANCE *instance);

void init_instance_by_config(INSTANCE *instance);

int comp_instance_node(const void *a, const void *b);
int	do_instance_action(ROLE *role, uint8_t type, struct instance_node *node);
uint32_t do_instance_use_item(ROLE *role, InstanceUseItemRequest *req);
#endif


