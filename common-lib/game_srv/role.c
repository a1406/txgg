#include <assert.h>
#include "role.h"
#include "sortarray.h"
#include "game_srv.h"
#include "game_net.h"
#include "special_proto.h"
#include "config_player.pb-c.h"
#include "config.h"

static int num_allroles_roleid;
static int num_allroles_fd;
static ROLE *allroles_roleid[max_role_in_game];   //通过roleid排序
static ROLE *allroles_fd[max_role_in_game];       //通过fd排序

inline int give_role_thing(ROLE *role, THING_OBJ *thing)
{
	return add_thing_obj((CONTAINER *)role->bag.p, thing);
}

inline int delete_role_thing(ROLE *role, THING_OBJ *thing)
{
	return delete_thing_obj((CONTAINER *)role->bag.p, thing);
}

inline int delete_role_thing_byid(ROLE *role, uint32_t id, uint16_t num)
{
	return delete_thing_byid((CONTAINER *)role->bag.p, id, num);
}

static int comp_role_by_roleid(const void *a, const void *b)
{
	assert(a);
	assert(b);

	if ((*(const ROLE **)a)->role_id == (*(const ROLE **)b)->role_id)
		return (0);
	else if ((*(const ROLE **)a)->role_id < (*(const ROLE **)b)->role_id)
		return (-1);
	return (1);
}

static int comp_role_by_fd(const void *a, const void *b)
{
	assert(a);
	assert(b);

	if ((*(const ROLE **)a)->fd == (*(const ROLE **)b)->fd)
		return (0);
	else if ((*(const ROLE **)a)->fd < (*(const ROLE **)b)->fd)
		return (-1);
	return (1);
}

ROLE *find_role_by_roleid(uint64_t role_id)
{
	ROLE role = {.role_id = role_id};
	ROLE *p = &role;
	int index, find;
	index = array_bsearch(&p, allroles_roleid, num_allroles_roleid, sizeof(ROLE*), &find, comp_role_by_roleid);
	if (find == 0)
		return NULL;
	assert(index < num_allroles_roleid);
	return allroles_roleid[index];
}

ROLE *find_role_by_fd(uint16_t fd)
{
	ROLE role = {.fd = fd};
	ROLE *p = &role;
	int index, find;
	index = array_bsearch(&p, allroles_fd, num_allroles_fd, sizeof(ROLE*), &find, comp_role_by_fd);
	if (find == 0)
		return NULL;
	assert(index < num_allroles_fd);
	return allroles_fd[index];
}

int add_one_role(ROLE *role)
{
	assert(role);
	if (num_allroles_roleid >= max_role_in_game)
		return (-1);
	if (num_allroles_fd >= max_role_in_game)
		return (-2);
	
	if ( array_insert(&role, allroles_roleid, &num_allroles_roleid, sizeof(ROLE *), 1, comp_role_by_roleid) < 0)
		return (-10);
	if ( array_insert(&role, allroles_fd, &num_allroles_fd, sizeof(ROLE *), 1, comp_role_by_fd) < 0) {
		array_delete(&role, allroles_roleid, &num_allroles_roleid, sizeof(ROLE *), comp_role_by_roleid);
		return (-20);
	}

	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: [%u] [%lu]", __FUNCTION__, role->fd, role->role_id);			
	return (0);
}

int del_one_role(ROLE *role)
{
	assert(role);
	if (0 != array_delete(&role, allroles_roleid, &num_allroles_roleid, sizeof(ROLE *), comp_role_by_roleid))
		return (-1);
	if (0 != array_delete(&role, allroles_fd, &num_allroles_fd, sizeof(ROLE *), comp_role_by_fd))
		return (-2);
	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: [%u] [%lu]", __FUNCTION__, role->fd, role->role_id);				
	return (0);
}

inline int role_logout(ROLE *role)
{
	THING_OBJ *thing;
	CONTAINER *bag;
	assert(role);
	
	if (0 != del_one_role(role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: del one role %lu failed", __FUNCTION__, role->role_id);		
		return (-10);
	}

	if (del_role_from_scene(role->scene.p, role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: del role [%u] [%lu] from scene failed", __FUNCTION__, role->fd, role->role_id);
//		return (-20);
	}
	
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: role [%u] [%lu] logout success", __FUNCTION__, role->fd, role->role_id);
	bag = (CONTAINER *)role->bag.p;
	if (bag) {
		//todo destory thingobj
		int i;
		for (i = 0; i < bag->max_thing_num; ++i) {
			thing = (THING_OBJ *)bag->things[i].p;
			if (thing)
				destory_thing_obj(thing);
		}
		destory_container(bag);
	} else {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role [%u] [%lu] logout without bag", __FUNCTION__, role->fd, role->role_id);
	}
	destory_role(role);	
	return (0);
}

inline void role_login(ROLE *role)
{
	assert(role);
	if (add_role_to_scene(role->scene.p, role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role [%u] [%lu] login failed", __FUNCTION__, role->fd, role->role_id);
		kick_role(role, 0);
		return;
	}


	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: role [%u] [%lu] login success", __FUNCTION__, role->fd, role->role_id);				
}

inline int kick_role(ROLE *role, uint32_t reason)
{
	KICK_NOTIFY *notify;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	assert(role);
	notify = (KICK_NOTIFY *)&head->data[0];
	notify->reason = reason;
	head->len = htons(sizeof(*head) + sizeof(KICK_NOTIFY));
	head->fd = role->fd;
	head->msg_id = htons(SERVER_PROTO_KICK_ROLE_NOTIFY);
	send_one_msg(conn_server.fd, (PROTO_HEAD *)head);
	return role_logout(role);
}

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
void test_add_role()
{
	int i;
	ROLE *role[100];
	srand(getpid());
	
	for (i = 0; i < 100; ++i) {
		role[i] = create_role();
		role[i]->role_id = rand();
		if (add_one_role(role[i]) == 0) {
			printf("add %lu\n", role[i]->role_id);
		} else {
			printf("add %lu failed\n", role[i]->role_id);			
		}
	}

	for (i = 0; i < 100; ++i) {
		if (!find_role_by_roleid(role[i]->role_id)) {
			printf("can not find, test failed\n");
		}
		if (del_one_role(role[i]) != 0) {
			printf("delete role failed\n");
		}
		if (find_role_by_roleid(role[i]->role_id)) {
			printf("can not delete, test failed\n");
		}		
	}
	exit(0);
}

void dump_role_summary(ROLE *role, RoleSummary *summary)
{
	assert(role);
	assert(summary);

	summary->profession = role->profession;
	summary->level = role->level;
	summary->name = role->name;
	summary->role_id = role->role_id & 0xffffffff;
	summary->area_id = role->role_id >> 32;
	summary->move_start_x = role->path.begin.pos_x;
	summary->move_start_y = role->path.begin.pos_y;
	summary->move_end_x = role->path.end.pos_x;
	summary->move_end_y = role->path.end.pos_y;		
}

void set_role_scene(ROLE *role, SCENE *scene)
{
	assert(role);
	assert(scene);

	role->scene.idx = GET_IDX(scene);
	role->scene.p = scene;

	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: set role scene [%u] [%lu] %u", __FUNCTION__, role->fd, role->role_id, scene->id);	
}

void set_role_bag(ROLE *role, CONTAINER *bag)
{
	assert(role);
	assert(bag);

	role->bag.idx = GET_IDX(bag);
	role->bag.p = bag;

	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: set role bag [%u] [%lu]", __FUNCTION__, role->fd, role->role_id);	
}

void init_role_data(ROLE *role)
{
	assert(role);
	role->name[0] = '\0';

	role->scene.idx = MAX_IDX;
	role->scene.p = NULL;
	role->instance.idx = MAX_IDX;
	role->instance.p = NULL;

	role->bag.idx = MAX_IDX;
	role->bag.p = NULL;
	role->store.idx = MAX_IDX;
	role->store.p = NULL;	
}

int resume_role_data(ROLE *role)
{
	SCENE *scene;
	INSTANCE *instance;
	CONTAINER *bag, *store;
	assert(role);

	scene = get_scene_byidx(role->scene.idx);
	if (!scene) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: fail", __FUNCTION__);
		return (-1);
	}
	role->scene.p = (void *)scene;

	if (role->instance.idx != MAX_IDX) {
		instance = get_instance_byidx(role->instance.idx);
		if (!instance) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: fail", __FUNCTION__);		
			return (-1);
		}
		role->instance.p = (void *)instance;
	}

	bag = get_container_byidx(role->bag.idx);
	if (!bag) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: fail", __FUNCTION__);		
		return (-1);
	}
	role->bag.p = (void *)bag;

	store = get_container_byidx(role->store.idx);
	if (!store) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: fail", __FUNCTION__);		
		return (-1);
	}
	role->store.p = (void *)store;
	
	return (0);
}

void reset_role_attribute(ROLE *role)
{
	CFGPLAYER *cfg;
	assert(role);
	cfg = get_player_config_byid(role->profession, role->level);
	if (!cfg) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: fail roleid = [%u][%lu] prof[%u] level[%u]", __FUNCTION__, role->profession, role->level);				
		return;
	}
	role->hp_max = cfg->hp;
	if (role->hp_cur > role->hp_max)
		role->hp_cur = role->hp_max;
	role->strength = cfg->strength;
	role->brain = cfg->brain;
	role->charm = cfg->charm;
	role->phy_attack = cfg->phy_attack;
	role->phy_defence = cfg->phy_defence;
	role->magic_attack = cfg->magic_attack;
	role->magic_defence = cfg->magic_defence;
	role->parry_rate = cfg->parry_rate;
	role->counterattack_rate = cfg->counterattack_rate;
	role->hit_rate = cfg->hit_rate;
	role->jink_rate = cfg->jink_rate;
	role->crit_rate = cfg->crit_rate;
	role->speed = cfg->speed;
}
