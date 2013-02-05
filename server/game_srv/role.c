#include <assert.h>
#include <errno.h>
#include "role.h"
#include "sortarray.h"
#include "game_srv.h"
#include "game_net.h"
#include "game_rand.h"
#include "quest.h"
#include "special_proto.h"
#include "config_player.pb-c.h"
#include "cs.pb-c.h"
#include "db_role_info.pb-c.h"
#include "config.h"
/*
static int num_allroles_roleid;
static int num_allroles_fd;
static ROLE *allroles_roleid[max_role_in_game];   //通过roleid排序
static ROLE *allroles_fd[max_role_in_game];       //通过fd排序
*/
struct data_head {
	struct rb_root head;
	comp_func comp;	
};

static struct data_head root_role_id_head;
static struct data_head root_role_name_head;
static struct data_head root_role_fd_head;


static inline void role_rbdata_init(ROLE *role)
{
	rb_init_node(&role->node_id);
	rb_init_node(&role->node_name);
	rb_init_node(&role->node_fd);		
}

inline static int comp_role_id(const void *a, const void *b)
{
	if ((uint64_t)a == (uint64_t)b)
		return (0);
	if ((uint64_t)a < (uint64_t)b)	
		return (-1);
	return (1);
}

inline static int comp_role_session(const void *a, const void *b)
{
	if (*(uint32_t *)a == *(uint32_t *)b)
		return (0);
	if (*(uint32_t *)a < *(uint32_t *)b)
		return (-1);
	return (1);
}

inline static int comp_role_name(const void *a, const void *b)
{
	return strcmp((char *)a, (char *)b);
}

void role_rbdata_init_head()
{
	root_role_id_head.head = RB_ROOT;
	root_role_name_head.head = RB_ROOT;
	root_role_fd_head.head = RB_ROOT;		

	root_role_id_head.comp = comp_role_id;
	root_role_name_head.comp = comp_role_name;
	root_role_fd_head.comp = comp_role_session;	
}

ROLE *get_role_by_session(uint32_t session)
{
	struct rb_node **p = &root_role_fd_head.head.rb_node;
	struct rb_node *parent = NULL;
	ROLE  *ptr;
	int ret;
	while (*p) {
		parent = *p;
		ptr = rb_entry(parent, ROLE, node_fd);
		ret = root_role_fd_head.comp((void *)&session, (void *)&ptr->session_id);
		if (ret == 0)
			return (ptr);
		else if (ret < 0)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	return (NULL);
	
}

ROLE *get_role_by_id(uint64_t id)
{
	struct rb_node **p = &root_role_id_head.head.rb_node;
	struct rb_node *parent = NULL;
	ROLE  *ptr;
	int ret;
	while (*p) {
		parent = *p;
		ptr = rb_entry(parent, ROLE, node_id);
		ret = root_role_id_head.comp((void *)id, (void *)ptr->role_id);
		if (ret == 0)
			return (ptr);
		else if (ret < 0)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	return (NULL);
	
}

ROLE *get_role_by_name(char *name)
{
	struct rb_node **p = &root_role_name_head.head.rb_node;
	struct rb_node *parent = NULL;
	ROLE  *ptr;
	int ret;
	while (*p) {
		parent = *p;
		ptr = rb_entry(parent, ROLE, node_name);
		ret = root_role_name_head.comp(name, ptr->one_role[0].attr.name);
		if (ret == 0)
			return (ptr);
		else if (ret < 0)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	return (NULL);
}

int role_rbdata_add_name(ROLE *role)
{
	struct data_head *head;	
	struct rb_node **p;
	struct rb_node *parent;
	ROLE  *ptr;
	int ret;

	head = &root_role_name_head;
	p = &head->head.rb_node;
	parent = NULL;

	while (*p) {
		parent = *p;
		ptr = rb_entry(parent, ROLE, node_name);
		ret = head->comp(role->one_role[0].attr.name, ptr->one_role[0].attr.name);
		if (ret == 0)
			return (-1);
		else if (ret < 0)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	rb_link_node(&role->node_name, parent, p);
	rb_insert_color(&role->node_name, &head->head);
	return (0);
}

int role_rbdata_add(ROLE *role)
{
	struct data_head *head;	
	struct rb_node **p;
	struct rb_node *parent;
	ROLE  *ptr;
	int ret;

	head = &root_role_id_head;
	p = &head->head.rb_node;
	parent = NULL;

	while (*p) {
		parent = *p;
		ptr = rb_entry(parent, ROLE, node_id);
		ret = head->comp((void *)role->role_id, (void *)ptr->role_id);
		if (ret == 0)
			return (-1);
		else if (ret < 0)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	rb_link_node(&role->node_id, parent, p);
	rb_insert_color(&role->node_id, &head->head);
	
	head = &root_role_fd_head;
	p = &head->head.rb_node;
	parent = NULL;

	while (*p) {
		parent = *p;
		ptr = rb_entry(parent, ROLE, node_fd);
		ret = head->comp((void *)&role->session_id, (void *)&ptr->session_id);
		if (ret == 0)
			return (-1);
		else if (ret < 0)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	rb_link_node(&role->node_fd, parent, p);
	rb_insert_color(&role->node_fd, &head->head);
	
	return (0);
}

void role_rbdata_del(ROLE *role)
{
	struct data_head *head;

	head = &root_role_id_head;	
	rb_erase(&role->node_id, &head->head);
	RB_CLEAR_NODE(&role->node_id);

	if (role->one_role[0].attr.name[0]) {
		head = &root_role_name_head;	
		rb_erase(&role->node_name, &head->head);
		RB_CLEAR_NODE(&role->node_name);
	}

	head = &root_role_fd_head;	
	rb_erase(&role->node_fd, &head->head);
	RB_CLEAR_NODE(&role->node_fd);		
}


static inline int comp_role_quest(const void *a, const void *b)
{
	if (((DB_QUEST_INFO *)a)->id == ((DB_QUEST_INFO *)b)->id)
		return (0);
	if (((DB_QUEST_INFO *)a)->id > ((DB_QUEST_INFO *)b)->id)	
		return (1);
	return (-1);
}

static inline int send_bag_changed_notify(ROLE *role, uint8_t pos)
{
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	THING_OBJ *thing;
	CONTAINER *bag;
	ContainerChangedNotify notify;
	ContainerThing container_thing;
	ContainerThing *container_thing_point[1];	
	
	container_thing_point[0] = &container_thing;
	container_thing__init(&container_thing);
	container_changed_notify__init(&notify);
	notify.changed_thing = container_thing_point;
	notify.n_changed_thing = 1;
	notify.container_type = CONTAINER__TYPE__BAG;

	bag = (CONTAINER *)role->bag.p;
	assert(bag);
	container_thing.thing_pos = pos;
	
	thing = get_thingobj_bypos(bag, pos);
	if (thing) {
		container_thing.thing_id = thing->id;
		container_thing.thing_num = thing->num;
	} else {
		container_thing.thing_id = 0;
		container_thing.thing_num = 0;		
	}
	
	size = container_changed_notify__pack(&notify, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__CONTAINER_CHANGED_NOTIFY);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (0);
}

inline int give_role_thing_bypos(ROLE *role, uint8_t pos, uint32_t id, uint16_t num)
{
	if (unlikely(add_thing_bypos((CONTAINER *)role->bag.p, pos, id, num)) != 0)
		return (-1);
	send_bag_changed_notify(role, pos);
	on_bag_changed(role);
	return (0);	
	
}
inline int delete_role_thing_bypos(ROLE *role, uint8_t pos, uint16_t num)
{
	if (unlikely(delete_thing_bypos((CONTAINER *)role->bag.p, pos, num)) != 0)
		return (-1);
	send_bag_changed_notify(role, pos);
	on_bag_changed(role);	
	return (0);	
}

inline int swap_role_thing_pos(ROLE *role, uint8_t pos1, uint8_t pos2)
{
	if (unlikely(swap_thing_pos((CONTAINER *)role->bag.p, pos1, pos2)) != 0)
		return (-1);
	send_bag_changed_notify(role, pos1);
	send_bag_changed_notify(role, pos2);	
	return (0);	
}

inline int give_role_thing_list(ROLE *role, struct thing_list *list)
{
	int ret;
	int i;

	ret = add_thing_list((CONTAINER *)role->bag.p, list);
	if (unlikely(ret != 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: role[%lu] ret[%d]", __FUNCTION__, role->role_id, ret);		
		return ret;
	}

	for (i = 0; i < list->num; ++i) {
		send_bag_changed_notify(role, list->things[i].pos);
	}
	on_bag_changed(role);	
	return (0);
}

/*
inline int delete_role_thing(ROLE *role, THING_OBJ *thing)
{
	int pos;
	pos = delete_thing_obj((CONTAINER *)role->bag.p, thing);
	if (unlikely(pos < 0))
		return pos;
	send_bag_changed_notify(role, pos);
	on_bag_changed(role);	
	return (0);	
}
*/
/*
inline int delete_role_thing_byid(ROLE *role, uint32_t id, uint16_t num)
{
	return delete_thing_byid((CONTAINER *)role->bag.p, id, num);
}
*/

/*
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
*/
int add_one_role(ROLE *role)
{
	assert(role);
	role_rbdata_add(role);	
/*
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
*/
	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: [%u] [%lu]", __FUNCTION__, role->session_id, role->role_id);			
	return (0);
}

int del_one_role(ROLE *role)
{
	assert(role);
/*	
	if (0 != array_delete(&role, allroles_roleid, &num_allroles_roleid, sizeof(ROLE *), comp_role_by_roleid))
		return (-1);
	if (0 != array_delete(&role, allroles_fd, &num_allroles_fd, sizeof(ROLE *), comp_role_by_fd))
		return (-2);
*/
	role_rbdata_del(role);	
	
	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: [%u] [%lu]", __FUNCTION__, role->session_id, role->role_id);				
	return (0);
}

inline int role_logout(ROLE *role)
{
	THING_OBJ *thing;
	CONTAINER *bag;
	INSTANCE *instance;	
	assert(role);

		//todo 下面的清理应该在db回复之后做
	send_store_role_request(role, active_roledb_srv);
	
	if (0 != del_one_role(role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: del one role %lu failed", __FUNCTION__, role->role_id);		
		return (-10);
	}

	if (del_role_from_scene(role->scene.p, role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: del role [%u] [%lu] from scene failed", __FUNCTION__, role->session_id, role->role_id);
//		return (-20);
	}
	
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: role [%u] [%lu] logout success", __FUNCTION__, role->session_id, role->role_id);
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
			"%s: role [%u] [%lu] logout without bag", __FUNCTION__, role->session_id, role->role_id);
	}

	instance = (INSTANCE *)role->instance.p;
	if (instance)
		leave_instance(role, instance);
	destory_role(role);	
	return (0);
}

inline void role_login(ROLE *role)
{
	assert(role);
	if (!is_role_friend_exist(role, 0))
		return;
	role_rbdata_add_name(role);
	if (add_role_to_scene(role->scene.p, role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role [%u] [%lu] login but can not add to scene", __FUNCTION__, role->session_id, role->role_id);
//		kick_role(role, 0);
		return;
	}


	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: role [%u] [%lu] login success", __FUNCTION__, role->session_id, role->role_id);				
}

inline int kick_role(ROLE *role, uint32_t reason)
{
	int len, ret;
	KICK_NOTIFY *notify;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	assert(role);
	notify = (KICK_NOTIFY *)&head->data[0];
	notify->reason = reason;
	len = sizeof(*head) + sizeof(KICK_NOTIFY);
	head->len = htons(len);
	head->fd = ROLE_FD(role);
	head->msg_id = htons(SERVER_PROTO_KICK_ROLE_NOTIFY);
	ret = send_to_role(role, (PROTO_HEAD *)head);
	if (ret != len) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send_one_msg fail len[%d] ret[%d]\n", __FUNCTION__, len, ret);
	}
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
		if (!get_role_by_id(role[i]->role_id)) {
			printf("can not find, test failed\n");
		}
		if (del_one_role(role[i]) != 0) {
			printf("delete role failed\n");
		}
		if (get_role_by_id(role[i]->role_id)) {
			printf("can not delete, test failed\n");
		}		
	}
	exit(0);
}

void dump_role_sight_info(ROLE *role, RoleSightInfo *info)
{
	assert(role);
	assert(info);

	info->profession = role->one_role[0].profession;
	info->level = role->one_role[0].attr.level;
	info->name = role->one_role[0].attr.name;
	info->role_id = role->role_id & 0xffffffff;
	info->area_id = role->role_id >> 32;
	info->move_start_x = role->path.begin.pos_x;
	info->move_start_y = role->path.begin.pos_y;
	info->move_end_x = role->path.end.pos_x;
	info->move_end_y = role->path.end.pos_y;
	info->move_speed = role->move_speed;			
}

void set_role_scene(ROLE *role, SCENE *scene)
{
	assert(role);
	assert(scene);

	role->scene.idx = GET_IDX(scene);
	role->scene.p = scene;

	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: set role scene [%u] [%lu] %u %p", __FUNCTION__, role->session_id, role->role_id, scene->id, scene);	
}

void set_role_store(ROLE *role, CONTAINER *store)
{
	assert(role);
	assert(store);

	role->store.idx = GET_IDX(store);
	role->store.p = store;

	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: set role store [%u] [%lu] %p", __FUNCTION__, role->session_id, role->role_id, store);		
}

void set_role_instance(ROLE *role, INSTANCE *instance)
{
	assert(role);
	assert(instance);

	role->instance.idx = GET_IDX(instance);
	role->instance.p = instance;

	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: set role instance [%u] [%lu] %p", __FUNCTION__, role->session_id, role->role_id, instance);	
}

void set_role_bag(ROLE *role, CONTAINER *bag)
{
	assert(role);
	assert(bag);

	role->bag.idx = GET_IDX(bag);
	role->bag.p = bag;

	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: set role bag [%u] [%lu] %p", __FUNCTION__, role->session_id, role->role_id, bag);	
}

void init_role_data(ROLE *role)
{
	assert(role);
	role->one_role[0].attr.name[0] = '\0';

	role->scene.idx = MAX_IDX;
	role->scene.p = NULL;
	role->instance.idx = MAX_IDX;
	role->instance.p = NULL;

	role->bag.idx = MAX_IDX;
	role->bag.p = NULL;
	role->store.idx = MAX_IDX;
	role->store.p = NULL;

	role->quest_num = 0;
	role_rbdata_init(role);	
}

int resume_role_data(ROLE *role)
{
	SCENE *scene;
	INSTANCE *instance;
	CONTAINER *bag, *store;
	assert(role);

	role_rbdata_init(role);

	if (!is_role_friend_exist(role, 0))	
		goto done;

	role_rbdata_add_name(role);		
	
	scene = get_scene_byidx(role->scene.idx);
	if (!scene) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: fail", __FUNCTION__);
		destory_role(role);
		return (-1);
	}
	set_role_scene(role, scene);

	if (role->instance.idx != MAX_IDX) {
		instance = get_instance_byidx(role->instance.idx);
		if (!instance) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: fail", __FUNCTION__);		
			return (-1);
		}
		set_role_instance(role, instance);
	}

	bag = get_container_byidx(role->bag.idx);
	if (!bag) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: fail", __FUNCTION__);		
		return (-1);
	}
	set_role_bag(role, bag);

	if (role->store.idx != MAX_IDX) {
		store = get_container_byidx(role->store.idx);
		if (!store) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: fail", __FUNCTION__);		
			return (-1);
		}
		set_role_store(role, store);
	}

done:	
	if (add_one_role(role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: add_one_role fail", __FUNCTION__);		
		return (-1);		
	}
	return (0);
}

void reset_role_attribute(ROLE *role)
{
	int i;
	assert(role);

	for (i = 0; i < MAX_FRIEND_PER_ROLE; ++i) {
		if (!is_role_friend_exist(role, i))			
			break;
		reset_one_role_attribute(&role->one_role[i]);
	}
}

void send_store_role_request(ROLE *role, CLIENT_MAP *roledb)
{
	PROTO_HEAD *head;
	CONTAINER *bag;
	SCENE *scene;	
	STORE_ROLE_DATA_REQUEST *request;
	size_t size, blob_size;
	int i;
	int ret;
	DB_ROLE_INFO *info;
	DbBlobRoleInfo blob_info;
	DbBag thing_list;
	DbQuestProgressList quest_list;

	DbShortcut shortcut[MAX_SHORTCUT_PER_ROLE];
	DbShortcut *shortcut_point[MAX_SHORTCUT_PER_ROLE];	
	
	DbQuestProgress quest[MAX_QUEST_PER_ROLE];
	DbQuestProgress *quest_point[MAX_QUEST_PER_ROLE];

	DbThing thing[MAX_THING_OBJ_PER_CONTAINER];
	DbThing *thing_point[MAX_THING_OBJ_PER_CONTAINER];

	DbOneRole one_role[MAX_FRIEND_PER_ROLE];
	DbOneRole *one_role_point[MAX_FRIEND_PER_ROLE];	

	DbFriend game_friend;
	assert(role);
	assert(roledb);

	if (!is_role_friend_exist(role, 0))					
		return;

	
	head = (PROTO_HEAD *)&toclient_send_buf[0];
	request = (STORE_ROLE_DATA_REQUEST *)&toclient_send_buf[sizeof(PROTO_HEAD)];

	bag = role->bag.p;
	scene = role->scene.p;

	if (!bag || !scene) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: fail roleid = [%u][%lu] prof[%u] level[%u] bag[%p] scene[%p]", __FUNCTION__, role->session_id, role->role_id,
			role->one_role[0].profession, role->one_role[0].attr.level, bag, scene);
		return;
	}
	
	size = sizeof(*request);
	info = &(request->info);

	request->id = role->role_id;
	info->profession = role->one_role[0].profession;
	info->level = role->one_role[0].attr.level;
	
	info->scene_id = scene->id;
	info->pos.pos_x = role->path.begin.pos_x;
	info->pos.pos_y = role->path.begin.pos_y;
	info->money = role->money;
	info->gold = role->gold;	

	size += sizeof(PROTO_HEAD);

	db_blob_role_info__init(&blob_info);
	db_bag__init(&thing_list);
	db_quest_progress_list__init(&quest_list);
	blob_info.bag = &thing_list;
	blob_info.quest = &quest_list;

	for (i = 0; i < role->quest_num; ++i) {
		quest_point[i] = &quest[i];
		db_quest_progress__init(quest_point[i]);
		quest[i].id = role->quest[i].id;
		quest[i].state = role->quest[i].state;
		quest[i].accept_time = role->quest[i].accept_time;
		quest[i].cycle_num = role->quest[i].cycle_num;
		quest[i].kill_npc1_num = role->quest[i].kill_npc1_num;
		quest[i].kill_npc2_num = role->quest[i].kill_npc2_num;
		quest[i].kill_npc3_num = role->quest[i].kill_npc3_num;
		quest[i].collect_item1_num = role->quest[i].collect_item1_num;
		quest[i].collect_item2_num = role->quest[i].collect_item2_num;
		quest[i].collect_item3_num = role->quest[i].collect_item3_num;
	}
	quest_list.quest = quest_point;
	quest_list.n_quest = role->quest_num;

	quest_list.main_quest_index = role->main_quest_index;
//	uint32_t slave_quest[role->finished_slave_quest_num];
	quest_list.n_all_finished_slave_quest = role->finished_slave_quest_num;
	quest_list.all_finished_slave_quest = role->all_finished_slave_quest;
//	for (i = 0; i < role->finished_slave_quest_num; ++i) {
//		slave_quest[i] = role->all_finished_slave_quest[i];
//	}


	blob_info.n_shortcut = MAX_SHORTCUT_PER_ROLE;
	blob_info.shortcut = shortcut_point;
	for (i = 0; i < MAX_SHORTCUT_PER_ROLE; ++i) {
		db_shortcut__init(&shortcut[i]);
		shortcut_point[i] = &shortcut[i];
		shortcut[i].id = role->shortcut[i].id;
		shortcut[i].pos = role->shortcut[i].pos;		
	}
	
	thing_list.n_things = 0;	
	for (i = 0; i < bag->max_thing_num; ++i) {
		if (!bag->things[i].p)
			continue;
		thing_point[thing_list.n_things] = &thing[i];
		db_thing__init(thing_point[thing_list.n_things]);
		thing[i].id = ((THING_OBJ *)(bag->things[i].p))->id;
		thing[i].num = ((THING_OBJ *)(bag->things[i].p))->num;
		thing[i].pos = i;
		++thing_list.n_things;
	}
	thing_list.things = thing_point;
	thing_list.max_num = bag->max_thing_num;

	for (i = 0; i < MAX_FRIEND_PER_ROLE; ++i) {
		if (!is_role_friend_exist(role, i))					
			break;
		one_role_point[i] = &one_role[i];
		db_one_role__init(&one_role[i]);
		one_role[i].n_equip = MAX_EQUIP_PER_ROLE;
		one_role[i].n_skill = role->one_role[i].skill_num;
		one_role[i].n_active_skill = MAX_ACTIVE_SKILL;		
		one_role[i].equip = role->one_role[i].equip;
		one_role[i].skill = role->one_role[i].skill;
		one_role[i].active_skill = role->one_role[i].attr.active_skill;

		one_role[i].name = role->one_role[i].attr.name;
		one_role[i].level = role->one_role[i].attr.level;
		one_role[i].prof = role->one_role[i].profession;
		one_role[i].exp = role->one_role[i].exp;
		
	}
	//至少要有个主角
	assert(i > 0);
	blob_info.n_one_role = i;
	blob_info.one_role = one_role_point;

	db_friend__init(&game_friend);
	blob_info.game_friend = &game_friend;

	game_friend.n_friend_ = get_role_game_friend_num(role);
	game_friend.friend_ = role->game_friend.friend;
	game_friend.n_blacklist = get_role_game_blacklist_num(role);
	game_friend.blacklist = role->game_friend.blacklist;
	
//	blob_info.n_skill = role->one_role[0].skill_num;
//	blob_info.skill = role->one_role[0].skill;

	blob_size = db_blob_role_info__pack(&blob_info, &info->blob_info[0]);
	info->blob_size = blob_size;
	size += blob_size;

	head->len = htons(size);
	head->msg_id = htons(SERVER_PROTO_STORE_ROLE_REQUEST);

	ret = send_one_msg(roledb->fd, head, 0);
	if (ret != size) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send_one_msg fail len[%u] ret[%d] errno[%d]\n", __FUNCTION__, size, ret, errno);
	}

}

int add_role_quest(ROLE *role, DB_QUEST_INFO *quest)
{
	int index;
	if (role->quest_num >= MAX_QUEST_PER_ROLE)
		return (-1);
	index = array_insert(quest, role->quest, (int *)&role->quest_num, sizeof(*quest), 1, comp_role_quest);
	return index;
}

int remove_role_quest(ROLE *role, uint32_t id)
{
	DB_QUEST_INFO data;
	data.id = id;	
	return array_delete(&data, role->quest, (int *)&role->quest_num, sizeof(data), comp_role_quest);

}
DB_QUEST_INFO *get_role_quest(ROLE *role, uint32_t id)
{
	int find, index;
	DB_QUEST_INFO data;
	data.id = id;

	index = array_bsearch(&data, role->quest, role->quest_num, sizeof(data), &find, comp_role_quest);
	if (find == 0)
		return NULL;
	assert(index < role->quest_num);
	return &role->quest[index];
}

static inline uint16_t get_role_conn_fd(ROLE *role)
{
	return conn_server[CONN_INDEX(role)].fd;
}

int send_to_role(ROLE *role, PROTO_HEAD *head)
{
	return send_one_msg(get_role_conn_fd(role), head, 0);
}

int role_modify_hp(ROLE *role, int hp)
{
	int i;
	AttrChangedNotify notify;
	AttrChanged attr[MAX_FRIEND_PER_ROLE], *attr_point[MAX_FRIEND_PER_ROLE];
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	assert(role);

	attr_changed_notify__init(&notify);
	notify.attr = attr_point;
	notify.role_id = role->role_id & 0xffffffff;
	notify.area_id = role->role_id >> 32;
	
	for (i = 0; i < MAX_FRIEND_PER_ROLE; ++i) {
		if (!is_role_friend_exist(role, i))
			break;
		role->one_role[i].attr.hp_cur += hp;
		if (role->one_role[i].attr.hp_cur < 0)
			role->one_role[i].attr.hp_cur = 0;
		else if (role->one_role[i].attr.hp_cur > role->one_role[i].attr.hp_max)
			role->one_role[i].attr.hp_cur = role->one_role[i].attr.hp_max;

		attr_changed__init(&attr[notify.n_attr]);
		attr_point[notify.n_attr] = &attr[notify.n_attr];
		attr[notify.n_attr].friend_pos = i;
		attr[notify.n_attr].id = ATTR__ID__HP_CUR;
		attr[notify.n_attr].new_value = role->one_role[i].attr.hp_cur;
		notify.n_attr++;
	}
	size = attr_changed_notify__pack(&notify, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__ATTR_CHANGED_NOTIFY);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);		
	}

	
	return (0);
}

int role_modify_lucky(ROLE *role, int drop, int money, int exp)
{
	assert(role);
	role->instance_data.lucky_drop += drop;
	role->instance_data.lucky_money += money;
	role->instance_data.lucky_exp += exp;	
	return (0);
}

uint16_t get_role_sceneid(ROLE *role)
{
	SCENE *scene;
	assert(role);
	scene = role->scene.p;
	if (!scene)
		return (0);
	return scene->id;
}

inline uint32_t get_role_equip_id(ROLE *role, int friend_pos, int equip_pos)
{
	if (friend_pos < 0 || friend_pos >= MAX_FRIEND_PER_ROLE)
		return (0);
	
	if (equip_pos < 0 || equip_pos >= MAX_EQUIP_PER_ROLE)
		return (0);
	return role->one_role[friend_pos].equip[equip_pos];
}
inline int set_role_equip_id(ROLE *role, int friend_pos, int equip_pos, uint32_t new_equip_id)
{
	assert(friend_pos >= 0 && friend_pos < MAX_FRIEND_PER_ROLE);		
	assert(equip_pos >= 0 && equip_pos < MAX_EQUIP_PER_ROLE);
	role->one_role[friend_pos].equip[equip_pos] = new_equip_id;
	return (0);	
}

inline int give_role_exp(ROLE *role, uint32_t exp)
{
	AttrChangedNotify notify;
	AttrChanged attr[MAX_FRIEND_PER_ROLE * 32], *attr_point[MAX_FRIEND_PER_ROLE * 32];
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;

	uint8_t dirty[MAX_FRIEND_PER_ROLE] = {0};
	int i;
	assert(role);

	attr_changed_notify__init(&notify);
	notify.attr = attr_point;
	notify.role_id = role->role_id & 0xffffffff;
	notify.area_id = role->role_id >> 32;
	
	for (i = 0; i < MAX_FRIEND_PER_ROLE; ++i) {
		if (!is_role_friend_exist(role, i))
			break;
		role->one_role[i].exp += exp;

		attr_changed__init(&attr[notify.n_attr]);
		attr[notify.n_attr].friend_pos = i;
		attr[notify.n_attr].id = ATTR__ID__EXP;
		attr[notify.n_attr].new_value = role->one_role[i].exp;				
		++notify.n_attr;
		
		for (;role->one_role[i].exp >= role->one_role[i].exp_levelup;) {
			++role->one_role[i].attr.level;
			reset_exp_levelup(&role->one_role[i]);
			dirty[i] = 1;
		}
	}

	notify.n_attr = 1;


		//todo 重新设置属性，并通知前台
	for (i = 0; i < MAX_FRIEND_PER_ROLE; ++i) {
		if (dirty[i] == 0)
			continue;
		reset_one_role_attribute(&role->one_role[i]);
		notify.n_attr += fill_one_role_attr_changed(&role->one_role[i], i, &attr[notify.n_attr]);		
	}

//	if (notify.n_attr == 0)
//		return (0);

	for (i = 0; i < notify.n_attr; ++i) {
		attr_point[i] = &attr[i];
	}
	
	size = attr_changed_notify__pack(&notify, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__ATTR_CHANGED_NOTIFY);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (0);
}

inline int send_attr_changed_notify(ROLE *role, int id, int new_value)
{
	AttrChangedNotify notify;
	AttrChanged attr, *attr_point;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	attr_changed_notify__init(&notify);
	attr_point = &attr;
	notify.n_attr = 1;
	notify.attr = &attr_point;
	attr_changed__init(&attr);
	notify.role_id = role->role_id & 0xffffffff;
	notify.area_id = role->role_id >> 32;

	attr.friend_pos = 0;
	attr.id = id;
	attr.new_value = new_value;

	size = attr_changed_notify__pack(&notify, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__ATTR_CHANGED_NOTIFY);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);
		return (-1);
	}
	return (0);
}

inline int modify_role_money(ROLE *role, int money)
{
	
	assert(role);
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: role[%lu] money is %u, add %d", __FUNCTION__, role->role_id, role->money, money);
	role->money += money;

	send_attr_changed_notify(role, ATTR__ID__MONEY, role->money);
	return (0);
}

inline int modify_role_gold(ROLE *role, int gold)
{
	assert(role);
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: role[%lu] gold is %u, add %d", __FUNCTION__, role->role_id, role->gold, gold);
	role->gold += gold;

	send_attr_changed_notify(role, ATTR__ID__GOLD, role->gold);	
	return (0);
}

inline int give_role_drop(ROLE *role, uint32_t drop_id, DropData *data)
{
	assert(role);
	int i;
	struct thing_list list;	
	CFGDROP *config = get_drop_config_byid(drop_id);
	int randnum;
	if (!config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%lu] drop config %u can not find", __FUNCTION__, role->role_id, drop_id);				
		return (-1);
	}

	if (config->exp) {
		give_role_exp(role, config->exp);
	}
	if (config->money) {
		modify_role_money(role, config->money);
	}
	if (config->gold) {
		modify_role_gold(role, config->gold);
	}

	if (data) {
		data->gold += config->gold;		
		data->money += config->money;		
		data->exp += config->exp;
	}

	randnum = get_rand_between(1, MAX_DROP_RATE);
	list.num = 0;
	for (i = 0; i < config->n_item; ++i) {
		if (config->item[i]->rate < randnum)
			continue;

		if (config->item[i]->id <= 0)
			continue;
		if (config->item[i]->num <= 0)
			continue;		
		
		if (list.num >= MAX_THINGLIST_NUM)
			break;
		
		list.things[list.num].id = config->item[i]->id;
		list.things[list.num].num = config->item[i]->num;
		++list.num;

		if (data && data->n_item_id < MAX_THINGLIST_NUM) {
			data->item_id[data->n_item_id] = config->item[i]->id;
			data->item_num[data->n_item_num] = config->item[i]->num;		
			++data->n_item_id;
			++data->n_item_num;
		}

		if (config->exclude)
			break;
		randnum = get_rand_between(1, MAX_DROP_RATE);		
	}

	
	if (list.num > 0)
		give_role_thing_list(role, &list);
	return (0);
}

inline uint8_t is_role_friend_exist(ROLE *role, uint8_t pos)
{
	assert(role);
	assert(pos < MAX_FRIEND_PER_ROLE);

	if (role->one_role[pos].attr.name[0] == '\0')
		return (0);
	return (1);
}

int add_role_game_friend(ROLE *role, uint64_t id)
{
	int i;
	ROLE *notify_role;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	FriendAddNotify notify;	
	assert(role);

	for (i = 0; i < MAX_GAME_FRIEND_PER_ROLE; ++i) {
		if (role->game_friend.friend[i] == id) {
			return (-1);
		}
		if (role->game_friend.friend[i] == 0) {
			role->game_friend.friend[i] = id;
			del_role_blacklist(role, id);
			
			notify_role = get_role_by_id(id);
			if (!notify_role)
				return (0);
			
			friend_add_notify__init(&notify);
			notify.role_id = role->role_id & 0xffffffff;
			notify.area_id = role->role_id >> 32;
			size = friend_add_notify__pack(&notify, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
			size += sizeof(PROTO_HEAD_CONN);
	
			head->msg_id = htons(CS__MESSAGE__ID__FRIEND_ADD_NOTIFY);
			head->len = htons(size);
			head->fd = ROLE_FD(notify_role);

			if (size != send_to_role(notify_role, (PROTO_HEAD *)head)) {
				log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
					"%s: send to %lu failed", __FUNCTION__, notify_role->role_id);		
			}
			
			return (0);
		}
	}
	return (-10);	
}

int del_role_game_friend(ROLE *role, uint64_t id)
{
	int i;
	assert(role);

	for (i = 0; i < MAX_GAME_FRIEND_PER_ROLE; ++i) {
		if (role->game_friend.friend[i] == id) {
			break;
		}
		if (role->game_friend.friend[i] == 0) {
			return (-1);
		}
	}
	if (i == MAX_GAME_FRIEND_PER_ROLE)
		return (-1);
	for (; i < MAX_GAME_FRIEND_PER_ROLE - 1; ++i) {
		role->game_friend.friend[i] = role->game_friend.friend[i + 1];
	}
	role->game_friend.friend[MAX_GAME_FRIEND_PER_ROLE - 1] = 0;
	return (0);	
}
int add_role_blacklist(ROLE *role, uint64_t id)
{
	int i;
	assert(role);

	for (i = 0; i < MAX_GAME_FRIEND_PER_ROLE; ++i) {
		if (role->game_friend.blacklist[i] == id) {
			return (-1);
		}
		if (role->game_friend.blacklist[i] == 0) {
			role->game_friend.blacklist[i] = id;
			del_role_game_friend(role, id);			
			return (0);
		}
	}
	return (-10);	
}

int del_role_blacklist(ROLE *role, uint64_t id)
{
	int i;
	assert(role);

	for (i = 0; i < MAX_GAME_FRIEND_PER_ROLE; ++i) {
		if (role->game_friend.blacklist[i] == id) {
			break;
		}
		if (role->game_friend.blacklist[i] == 0) {
			return (-1);
		}
	}
	if (i == MAX_GAME_FRIEND_PER_ROLE)
		return (-1);
	for (; i < MAX_GAME_FRIEND_PER_ROLE - 1; ++i) {
		role->game_friend.blacklist[i] = role->game_friend.blacklist[i + 1];
	}
	role->game_friend.blacklist[MAX_GAME_FRIEND_PER_ROLE - 1] = 0;
	return (0);	
}

int get_role_game_friend_num(ROLE *role)
{
	int i;
	assert(role);
	for (i = 0; i < MAX_GAME_FRIEND_PER_ROLE; ++i) {
		if (role->game_friend.friend[i] == 0)
			break;
	}
	return (i);
}
int get_role_game_blacklist_num(ROLE *role)
{
	int i;
	assert(role);
	for (i = 0; i < MAX_GAME_FRIEND_PER_ROLE; ++i) {
		if (role->game_friend.blacklist[i] == 0)
			break;
	}
	return (i);
}
