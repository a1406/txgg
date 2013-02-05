#include <assert.h>
#include "game_srv.h"
#include "container.h"
#include "thing_obj.h"
#include "config.h"

static inline int set_container_thingobj(CONTAINER *container, THING_OBJ *thing, uint8_t pos)
{
	assert(container);
	if (pos >= container->max_thing_num) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: pos invalid %lu", __FUNCTION__, pos);				
		return (-1);
	}
	container->things[pos].p = thing;
	if (thing)
		container->things[pos].idx = GET_IDX(thing);
	else
		container->things[pos].idx = MAX_IDX;
	return (0);
}

static int add_stack_thing(CONTAINER *container, struct things *thing, CFGITEM *config)
{
	int i;
	THING_OBJ *thing_bag;	
	for (i = 0; i < container->max_thing_num; ++i) {
		if (!container->things[i].p)
			continue;
		thing_bag = (THING_OBJ *)container->things[i].p;
		if (thing_bag->id == thing->id && thing->num + thing_bag->num <= config->stack_able) {
			thing->pos = i;
			return (0);
		}
	}
	return (-1);
}

static int add_unstack_thing(CONTAINER *container, struct things *thing, uint8_t begin)
{
	for (; begin < container->max_thing_num; ++begin) {
		if (!container->things[begin].p) {
			thing->pos = begin;
			return (begin + 1);
		}
	}
	return (-1);
}

int add_thing_list(CONTAINER *container, struct thing_list *list)
{
	int i, j;
	int empty_pos = 0;
	CFGITEM *config;
	assert(container);
	assert(list);

	for (i = 0; i < list->num; ++i) {
		config = get_item_config_byid(list->things[i].id);
		if (!config) {
			return (-1);
		}
		if (config->stack_able > 1 && add_stack_thing(container, &list->things[i], config) == 0) {
			continue;
		}
		empty_pos = add_unstack_thing(container, &list->things[i], empty_pos);
		if (empty_pos < 0) {
			return (-10);
		}
	}

	for (i = 0; i < list->num; ++i) {
		if (add_thing_bypos(container, list->things[i].pos,
				list->things[i].id, list->things[i].num) != 0)
			goto fail;
	}
	return (0);
fail:
	for (j = 0; j < j; ++j) {
		delete_thing_bypos(container, list->things[j].pos, list->things[j].num);
	}
	return (-30);
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

int add_thing_bypos(CONTAINER *container, uint8_t pos, uint32_t id, uint16_t num)
{
	THING_OBJ *thing;
	assert(container);
	if (pos >= container->max_thing_num) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: pos invalid %lu", __FUNCTION__, pos);		
		return (-1);
	}
	thing = (THING_OBJ *)container->things[pos].p;
	if (!thing) {
		thing = create_thing_obj();
		if (unlikely(!thing)) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: create thing obj fail", __FUNCTION__);		
			return (-1);
		}
		thing->id = id;
		thing->num = num;
		set_container_thingobj(container, thing, pos);
	} else if (thing->id == id){
		thing->num += num;		
	} else {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: [%d] thing id[%u] != %u %lu", __FUNCTION__, pos, thing->id, id);		
		return (-1);		
	}
	return (0);
}

int delete_thing_bypos(CONTAINER *container, uint8_t pos, uint16_t num)
{
	THING_OBJ *thing;
	assert(container);
	if (pos >= container->max_thing_num) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: pos invalid %lu", __FUNCTION__, pos);		
		return (-1);
	}
	thing = (THING_OBJ *)container->things[pos].p;
	if (!thing || thing->num < num) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: thing invalid %lu", __FUNCTION__, pos);				
		return (-10);
	}
	thing->num -= num;

	if (thing->num == 0) {
		destory_thing_obj(thing);
		container->things[pos].idx = MAX_IDX;
		container->things[pos].p = NULL;		
	}
	return (0);
}

THING_OBJ *get_thingobj_bypos(CONTAINER *container, uint8_t pos)
{
	THING_OBJ *thing;	
	assert(container);
	if (pos >= container->max_thing_num) {
		return NULL;
	}
	thing = (THING_OBJ *)container->things[pos].p;
	if (!thing || thing->num <= 0) {
		return (NULL);
	}
	return thing;
}


int swap_thing_pos(CONTAINER *container, uint8_t pos1, uint8_t pos2)
{
	THING_OBJ *thing;
//	assert(0);
	if (pos1 >= container->max_thing_num) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: pos invalid %lu", __FUNCTION__, pos1);
		return (-1);
	}
	if (pos2 >= container->max_thing_num) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: pos invalid %lu", __FUNCTION__, pos2);		
		return (-1);
	}
	thing = container->things[pos1].p;

	set_container_thingobj(container, container->things[pos2].p, pos1);
	set_container_thingobj(container, thing, pos2);	

	return (0);
}

