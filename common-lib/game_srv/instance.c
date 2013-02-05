#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "instance.h"
#include "sortarray.h"
#include "game_srv.h"

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
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s %d: role[%u] already in instance[%d]",
			__FUNCTION__, __LINE__, idx_role.idx, instance->id);				
		return (-10);
	}
	role->instance.idx = GET_IDX(instance);
	role->instance.p = instance;
	role->instance_dice = 0;

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
	return (ret);
}

void init_instance_data(INSTANCE *instance)
{
	assert(instance);
	instance->role_num = 0;
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
