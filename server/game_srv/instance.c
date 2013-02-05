#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "instance.h"
#include "config.h"
#include "sortarray.h"
#include "game_srv.h"
#include "game_net.h"
#include "fight.h"
#include "quest.h"
#include "keyword.pb-c.h"

int enter_instance(ROLE *role, INSTANCE *instance)
{
	int ret;
	IDX idx_role;
	assert(instance);
	assert(role);
	assert(role->instance.p == NULL);
	if (instance->role_num >= MAX_ROLE_PER_INSTANCE) {
		return (-1);
	}

	idx_role.idx = GET_IDX(role);
	idx_role.p = role;
	ret = array_insert(&idx_role, &instance->role[0], (int *)&instance->role_num, sizeof(IDX), 1, comp_idx);
	
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: role[%u] already in instance[%d]",
			__FUNCTION__, __LINE__, idx_role.idx, instance->id);				
		return (-10);
	}
	set_role_instance(role, instance);
	memset(&role->instance_data, 0, sizeof(role->instance_data));
/*
	role->instance_data.dice = 0;
	role->instance_data.action = 0;
	role->instance_data.back_steps = 0;
	role->instance_data.seq_id = 0;
	role->instance_data.special_dice = 0;
*/	
		//todo  role->instance_pos

	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG, "%s: role[%lu] enter instance[%d]",
		__FUNCTION__, role->role_id, instance->id);
	
	return (0);
}
int leave_instance(ROLE *role, INSTANCE *instance)
{
	int ret;
	IDX idx_role;	
	assert(instance);
	assert(role);

	idx_role.idx = GET_IDX(role);
	idx_role.p = role;
	ret = array_delete(&idx_role, &instance->role[0], (int *)&instance->role_num, sizeof(IDX), comp_idx);
	
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s %d: role[%u] not in instance[%d]",
			__FUNCTION__, __LINE__, idx_role.idx, instance->id);				
		return (-10);
	}

	role->instance.idx = MAX_IDX;
	role->instance.p = NULL;

	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG, "%s: role[%lu] leave instance[%d]",
		__FUNCTION__, role->role_id, instance->id);					

	if (instance->role_num == 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG, "%s: destory instance[%d]",
			__FUNCTION__, role->role_id, instance->id);							
		destory_instance(instance);
	}
	return (0);
}

INSTANCE *new_instance(uint16_t id)
{
	INSTANCE *ret = create_instance();
	if (!ret) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s %d: create_instance failed");
		return (NULL);
	}
	ret->id = id;
	ret->role_num = 0;
	init_instance_by_config(ret);	
	return (ret);
}

int comp_instance_node(const void *a, const void *b)
{
	if (((struct instance_node *)a)->id == ((struct instance_node *)b)->id)
		return (0);
	if (((struct instance_node *)a)->id > ((struct instance_node *)b)->id)
		return (1);
	return (-1);
}

void init_instance_by_config(INSTANCE *instance)
{
	int i;
	CFGINSTANCEMAP *map;
	map = get_instance_map_config_byid(instance->id);
	if (unlikely(!map)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: can not find instance[%d] map", __FUNCTION__, instance->id);
		return;
	}
	for (i = 0; i < map->n_node; ++i) {
		instance->node[i].id = map->node[i]->id & 0xffff;
		instance->node[i].seq_id = map->node[i]->seq_id;
		instance->node[i].action = map->node[i]->action;
		instance->node[i].up_pos = map->node[i]->up_pos;
		instance->node[i].down_pos = map->node[i]->down_pos;
		instance->node[i].left_pos = map->node[i]->left_pos;
		instance->node[i].right_pos = map->node[i]->right_pos;
	}
	instance->node_num = map->n_node;
}

void init_instance_data(INSTANCE *instance)
{
//	struct instance_node node;
	assert(instance);
	instance->role_num = 0;
	instance->node_num = 0;	
}

int resume_instance_data(INSTANCE *instance)
{
	int i;
	assert(instance);
	for (i = 0; i < instance->role_num; ++i) {
		instance->role[i].p = (void *)get_role_byidx(instance->role[i].idx);
		if (!instance->role[i].p) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: fail", __FUNCTION__);			
			return (-1);
		}
	}
	return (0);
}


static int send_fight_notify(ROLE *role, FightNotify *notify)
{
	if (!notify || !role)
		return (0);

	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;

	size = fight_notify__pack(notify, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);

	head->msg_id = htons(CS__MESSAGE__ID__INSTANCE_FIGHT_NOTIFY);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send to %lu failed", __FUNCTION__, role->role_id);		
	}
	
	return (0);
}

int send_backward_notify(ROLE *role, int num)
{
	InstanceBackwardNotify notify;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;

	instance_backward_notify__init(&notify);
	notify.back_num = num;
	size = instance_backward_notify__pack(&notify, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__INSTANCE_BACKWARD_NOTIFY);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send to %lu failed", __FUNCTION__, role->role_id);		
	}
	
	return (0);
}

int	do_instance_action(ROLE *role, uint8_t type, struct instance_node *node)
{
	int i;
	CFGMAPFUNCTION *func;	
	FightNotify *notify = NULL;

	assert(role);
	assert(node);	

	if (type == CFG__KEYWORD__NAME__instance_node_type_null)
		goto done;

	func = get_instance_function_byid(node->action);
	if (unlikely(!func)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: fail, id = %u, action = %u", __FUNCTION__,
			role->instance_data.seq_id, node->action);		
		goto done;
	}
	
	switch (type)
	{
		case CFG__KEYWORD__NAME__instance_node_type_boss:
			notify = test_fight(role, node->action);
			if (notify && notify->result == 0) {
				for (i = 0; i <func->n_drop; ++i) {
					give_role_drop(role, func->drop[i], notify->drop_data);
				}
				role->instance_data.state = INSTANCE_STATE_FINISHED;
				on_kill_monster(role, node->action);
			}
			send_fight_notify(role, notify);
			break;
		case CFG__KEYWORD__NAME__instance_node_type_elite:    //精英
			break;
		case CFG__KEYWORD__NAME__instance_node_type_fight:  //战斗
			if (role->instance_data.nowar_times == 0) {
				notify = test_fight(role, node->action);
				if (notify && notify->result == 0) {
					for (i = 0; i <func->n_drop; ++i) {
						give_role_drop(role, func->drop[i], notify->drop_data);
					}
					on_kill_monster(role, node->action);
				}				
			}
			send_fight_notify(role, notify);
			break;
		case CFG__KEYWORD__NAME__instance_node_type_heal:   //治疗
		case CFG__KEYWORD__NAME__instance_node_type_trap:   //陷阱
			role_modify_hp(role, func->hp);
			break;
		case CFG__KEYWORD__NAME__instance_node_type_luck:   //幸运
			
			break;
		case CFG__KEYWORD__NAME__instance_node_type_rand:   //随机
			break;
		case CFG__KEYWORD__NAME__instance_node_type_forward: //前进
			break;
		case CFG__KEYWORD__NAME__instance_node_type_backward:  //后退
			role->instance_data.back_steps = func->move;
			role->instance_data.dice = func->move;			
			send_backward_notify(role, func->move);
			break;
		case CFG__KEYWORD__NAME__instance_node_type_transport:  //传送
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: role  %lu should not stop at transport node", __FUNCTION__, role->role_id);
			goto done;
	}
	node->action = CFG__KEYWORD__NAME__instance_node_type_null;
	role->instance_data.action = 0;

done:
	if (unlikely(role->instance_data.nowar_times))
		--role->instance_data.nowar_times;
	return (0);
}

uint32_t do_instance_use_item(ROLE *role, InstanceUseItemRequest *req)
{
	switch (req->item_id)
	{
		case CFG__KEYWORD__NAME__special_item_id_dice:
			role->instance_data.dice = req->item_param;			
			break;
		case CFG__KEYWORD__NAME__special_item_id_nowar:
			role->instance_data.nowar_times = 3;
			break;
		default:
			return (-1);
			
	}
	return (0);
}
