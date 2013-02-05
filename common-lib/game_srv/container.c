#include <assert.h>
#include "game_srv.h"
#include "container.h"
#include "thing_obj.h"

int add_thing_obj(CONTAINER *container, THING_OBJ *thing)
{
	assert(container);
	assert(thing);
	int i;
	int first_pos = -1;
	THING_OBJ *thing_bag;

	for (i = 0 ; i < container->max_thing_num; ++i) {
		if (container->things[i].idx == MAX_IDX) {
			if (first_pos < 0)
				first_pos = i;
			continue;
		}
		
		assert(container->things[i].p);
		thing_bag = container->things[i].p;
		if (thing_bag->id != thing->id)
			continue;
		thing_bag->num += thing->num;
		return (0);
	}

	if (first_pos < 0)
		return (-1);
	container->things[first_pos].idx = GET_IDX(thing);
	container->things[first_pos].p = thing;
	return (0);
}

int delete_thing_obj(CONTAINER *container, THING_OBJ *thing)
{
	int i;
	assert(container);
	assert(thing);
	for (i = 0 ; i < container->max_thing_num; ++i) {
		if (container->things[i].idx == MAX_IDX)
			continue;
		assert(container->things[i].p);
		if (thing == container->things[i].p) {
			assert (container->things[i].idx != MAX_IDX);
			destory_thing_obj(thing);
			container->things[i].idx = MAX_IDX;
			container->things[i].p = NULL;		
			return (0);
		}
	}
	return (-1);
}

int count_thing_num(CONTAINER *container, uint32_t id)
{
	assert(container);
	int i;
	THING_OBJ *thing;
	int ret = 0;
	
	for (i = 0 ; i < container->max_thing_num; ++i) {
		if (container->things[i].idx == MAX_IDX)
			continue;
		assert(container->things[i].p);
		thing = container->things[i].p;
		if (thing->id != id)
			continue;
		ret += thing->num;
	}
	return (ret);
}

int delete_thing_byid(CONTAINER *container, uint32_t id, uint16_t num)
{
	assert(container);
	THING_OBJ *thing;	
	int num_available, i;
	
	num_available = count_thing_num(container, id);
	if (num_available < num) {
		return (-1);
	}
	for (i = 0 ; i < container->max_thing_num; ++i) {
		if (container->things[i].idx == MAX_IDX)
			continue;
		assert(container->things[i].p);
		thing = container->things[i].p;
		if (thing->id != id)
			continue;
		if (thing->num > num) {
			thing->num -= num;
			return (0);
		}
		destory_thing_obj(thing);
		container->things[i].idx = MAX_IDX;
		container->things[i].p = NULL;		
		num -= thing->num;
	}
	return (0);
}

void init_container_data(CONTAINER *container)
{
	int i;
	assert(container);
	for (i = 0; i < MAX_THING_OBJ_PER_CONTAINER; ++i) {
		container->things[i].idx = MAX_IDX;
		container->things[i].p = NULL;		
	}
}
int resume_container_data(CONTAINER *container)
{
	int i;
	assert(container);
	for (i = 0; i < container->max_thing_num; ++i) {
		if (container->things[i].idx == MAX_IDX)
			continue;
		container->things[i].p = (void *)get_thing_obj_byidx(container->things[i].idx);		
	}
	return (0);
}
