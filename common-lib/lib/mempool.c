#include "game_queue.h"
#include "mempool.h"
#include <string.h>
#include <assert.h>
#include <sys/ipc.h>
#include <sys/shm.h>


/*
const int  max_container_in_game =   (32);
const int max_thing_obj_in_game =  (10240);
const int max_role_in_game =  (4096);
const int max_scene_in_game =  (4096);
const int max_instance_in_game =  (4096);


POOL_DEF(container);
POOL_DEF(thing_obj)
POOL_DEF(role)
POOL_DEF(scene)
POOL_DEF(instance)

const static key_t key = 0x10001010;
*/
#if 0



#define MAX_CONTAINER_IN_GAME   (32)
#define MAX_THING_OBJ_IN_GAME   (10240)
#define MAX_ROLE_OBJ_IN_GAME   (4096)
#define MAX_SCENE_OBJ_IN_GAME   (4096)
#define MAX_INSTANCE_OBJ_IN_GAME   (4096)


struct pool_container_head *pool_container_used, *pool_container_freed;
static CONTAINER *pool_container;

struct pool_thing_obj_head *pool_thing_obj_used, *pool_thing_obj_freed;
static THING_OBJ *pool_thing_obj;

struct pool_role_obj_head *pool_role_obj_used, *pool_role_obj_freed;
static ROLE *pool_role_obj;

struct pool_scene_obj_head *pool_scene_obj_used, *pool_scene_obj_freed;
static SCENE *pool_scene_obj;

struct pool_instance_obj_head *pool_instance_obj_used, *pool_instance_obj_freed;
static INSTANCE *pool_instance_obj;

void init_container()
{
	int i;
	GAME_CIRCLEQ_INIT(pool_container_used);
	GAME_CIRCLEQ_INIT(pool_container_freed);
	for (i = 0; i < MAX_CONTAINER_IN_GAME; ++i) {
		pool_container[i].base.list_free.idx = i;
		pool_container[i].base.list_used.idx = i;		
		GAME_CIRCLEQ_INSERT_HEAD(pool_container_freed, &pool_container[i], base.list_free);
	}
}

CONTAINER *create_container()
{
	CONTAINER *first = pool_container_freed->cqh_first;
	if (!first || first == (CONTAINER *)pool_container_freed)
		return NULL;
	GAME_CIRCLEQ_REMOVE(pool_container_freed, first, base.list_free);
	GAME_CIRCLEQ_INSERT_HEAD(pool_container_used, first, base.list_used);
	return first;
}
void destory_container(CONTAINER* p)
{
	assert(p);
	GAME_CIRCLEQ_REMOVE(pool_container_used, p, base.list_used);
	GAME_CIRCLEQ_INSERT_HEAD(pool_container_freed, p, base.list_free);	
}

static inline CONTAINER *get_container_byidx(uint32_t idx)
{
	if (idx < MAX_CONTAINER_IN_GAME)
		return &pool_container[idx];
	return NULL;
}

//恢复container的链表
int resume_container()
{
	int i;
	for (i = 0; i < MAX_CONTAINER_IN_GAME; ++i) {
		assert(pool_container[i].base.list_free.idx == i);
		assert(pool_container[i].base.list_used.idx == i);
		pool_container[i].base.list_free.cqe_next =
			get_container_byidx(pool_container[i].base.list_free.idx_next);
		if (!pool_container[i].base.list_free.cqe_next)
			pool_container[i].base.list_free.cqe_next = (CONTAINER *)pool_container_freed;
		
		pool_container[i].base.list_free.cqe_prev =
			get_container_byidx(pool_container[i].base.list_free.idx_prev);;
		if (!pool_container[i].base.list_free.cqe_prev)
			pool_container[i].base.list_free.cqe_prev = (CONTAINER *)pool_container_freed;
		
		pool_container[i].base.list_used.cqe_next =
			get_container_byidx(pool_container[i].base.list_used.idx_next);
		if (!pool_container[i].base.list_used.cqe_next)
			pool_container[i].base.list_used.cqe_next = (CONTAINER *)pool_container_used;
		
		pool_container[i].base.list_used.cqe_prev =
			get_container_byidx(pool_container[i].base.list_used.idx_prev);
		if (!pool_container[i].base.list_used.cqe_prev)
			pool_container[i].base.list_used.cqe_prev = (CONTAINER *)pool_container_used;
	}

	pool_container_freed->cqh_first = get_container_byidx(pool_container_freed->idx_first);
	if (!pool_container_freed->cqh_first)
		pool_container_freed->cqh_first = (CONTAINER *)pool_container_freed;
	pool_container_freed->cqh_last = get_container_byidx(pool_container_freed->idx_last);
	if (!pool_container_freed->cqh_last)
		pool_container_freed->cqh_last = (CONTAINER *)pool_container_freed;

	pool_container_used->cqh_first = get_container_byidx(pool_container_used->idx_first);
	if (!pool_container_used->cqh_first)
		pool_container_used->cqh_first = (CONTAINER *)pool_container_used;
	pool_container_used->cqh_last = get_container_byidx(pool_container_used->idx_last);
	if (!pool_container_used->cqh_last)
		pool_container_used->cqh_last = (CONTAINER *)pool_container_used;	
	return (0);
}

void init_thing_obj()
{
	int i;
	GAME_CIRCLEQ_INIT(pool_thing_obj_used);
	GAME_CIRCLEQ_INIT(pool_thing_obj_freed);
	for (i = 0; i < MAX_THING_OBJ_IN_GAME; ++i) {
		pool_thing_obj[i].base.list_free.idx = i;
		pool_thing_obj[i].base.list_used.idx = i;				
		GAME_CIRCLEQ_INSERT_HEAD(pool_thing_obj_freed, &pool_thing_obj[i], base.list_free);
	}	
}

THING_OBJ *create_thing_obj()
{
	THING_OBJ *first = pool_thing_obj_freed->cqh_first;
	if (!first || first == (THING_OBJ *)pool_thing_obj_freed)
		return NULL;	
	GAME_CIRCLEQ_REMOVE(pool_thing_obj_freed, first, base.list_free);
	GAME_CIRCLEQ_INSERT_HEAD(pool_thing_obj_used, first, base.list_used);
	return first;	
}
void destory_thing_obj(THING_OBJ *p)
{
	assert(p);
	GAME_CIRCLEQ_REMOVE(pool_thing_obj_used, p, base.list_used);
	GAME_CIRCLEQ_INSERT_HEAD(pool_thing_obj_freed, p, base.list_free);		
}


static inline THING_OBJ *get_thing_obj_byidx(uint32_t idx)
{
	if (idx < MAX_THING_OBJ_IN_GAME)
		return &pool_thing_obj[idx];
	return NULL;
}


int resume_thing_obj(uint32_t idx)
{
	int i;
	for (i = 0; i < MAX_THING_OBJ_IN_GAME; ++i) {
		assert(pool_thing_obj[i].base.list_free.idx == i);
		assert(pool_thing_obj[i].base.list_used.idx == i);
		pool_thing_obj[i].base.list_free.cqe_next =
			get_thing_obj_byidx(pool_thing_obj[i].base.list_free.idx_next);
		if (!pool_thing_obj[i].base.list_free.cqe_next)
			pool_thing_obj[i].base.list_free.cqe_next = (THING_OBJ *)pool_thing_obj_freed;
		
		pool_thing_obj[i].base.list_free.cqe_prev =
			get_thing_obj_byidx(pool_thing_obj[i].base.list_free.idx_prev);;
		if (!pool_thing_obj[i].base.list_free.cqe_prev)
			pool_thing_obj[i].base.list_free.cqe_prev = (THING_OBJ *)pool_thing_obj_freed;
		
		pool_thing_obj[i].base.list_used.cqe_next =
			get_thing_obj_byidx(pool_thing_obj[i].base.list_used.idx_next);
		if (!pool_thing_obj[i].base.list_used.cqe_next)
			pool_thing_obj[i].base.list_used.cqe_next = (THING_OBJ *)pool_thing_obj_used;
		
		pool_thing_obj[i].base.list_used.cqe_prev =
			get_thing_obj_byidx(pool_thing_obj[i].base.list_used.idx_prev);
		if (!pool_thing_obj[i].base.list_used.cqe_prev)
			pool_thing_obj[i].base.list_used.cqe_prev = (THING_OBJ *)pool_thing_obj_used;
	}

	pool_thing_obj_freed->cqh_first = get_thing_obj_byidx(pool_thing_obj_freed->idx_first);
	if (!pool_thing_obj_freed->cqh_first)
		pool_thing_obj_freed->cqh_first = (THING_OBJ *)pool_thing_obj_freed;
	pool_thing_obj_freed->cqh_last = get_thing_obj_byidx(pool_thing_obj_freed->idx_last);
	if (!pool_thing_obj_freed->cqh_last)
		pool_thing_obj_freed->cqh_last = (THING_OBJ *)pool_thing_obj_freed;

	pool_thing_obj_used->cqh_first = get_thing_obj_byidx(pool_thing_obj_used->idx_first);
	if (!pool_thing_obj_used->cqh_first)
		pool_thing_obj_used->cqh_first = (THING_OBJ *)pool_thing_obj_used;
	pool_thing_obj_used->cqh_last = get_thing_obj_byidx(pool_thing_obj_used->idx_last);
	if (!pool_thing_obj_used->cqh_last)
		pool_thing_obj_used->cqh_last = (THING_OBJ *)pool_thing_obj_used;	
	return (0);
}

void init_role_obj()
{
	int i;
	GAME_CIRCLEQ_INIT(pool_role_obj_used);
	GAME_CIRCLEQ_INIT(pool_role_obj_freed);
	for (i = 0; i < MAX_ROLE_OBJ_IN_GAME; ++i) {
		pool_role_obj[i].base.list_free.idx = i;
		pool_role_obj[i].base.list_used.idx = i;				
		GAME_CIRCLEQ_INSERT_HEAD(pool_role_obj_freed, &pool_role_obj[i], base.list_free);
	}	
}

ROLE *create_role_obj()
{
	ROLE *first = pool_role_obj_freed->cqh_first;
	if (!first || first == (ROLE *)pool_role_obj_freed)
		return NULL;	
	GAME_CIRCLEQ_REMOVE(pool_role_obj_freed, first, base.list_free);
	GAME_CIRCLEQ_INSERT_HEAD(pool_role_obj_used, first, base.list_used);
	return first;	
}
void destory_role_obj(ROLE *p)
{
	assert(p);
	GAME_CIRCLEQ_REMOVE(pool_role_obj_used, p, base.list_used);
	GAME_CIRCLEQ_INSERT_HEAD(pool_role_obj_freed, p, base.list_free);		
}


static inline ROLE *get_role_obj_byidx(uint32_t idx)
{
	if (idx < MAX_ROLE_OBJ_IN_GAME)
		return &pool_role_obj[idx];
	return NULL;
}


int resume_role_obj(uint32_t idx)
{
	int i;
	for (i = 0; i < MAX_ROLE_OBJ_IN_GAME; ++i) {
		assert(pool_role_obj[i].base.list_free.idx == i);
		assert(pool_role_obj[i].base.list_used.idx == i);
		pool_role_obj[i].base.list_free.cqe_next =
			get_role_obj_byidx(pool_role_obj[i].base.list_free.idx_next);
		if (!pool_role_obj[i].base.list_free.cqe_next)
			pool_role_obj[i].base.list_free.cqe_next = (ROLE *)pool_role_obj_freed;
		
		pool_role_obj[i].base.list_free.cqe_prev =
			get_role_obj_byidx(pool_role_obj[i].base.list_free.idx_prev);;
		if (!pool_role_obj[i].base.list_free.cqe_prev)
			pool_role_obj[i].base.list_free.cqe_prev = (ROLE *)pool_role_obj_freed;
		
		pool_role_obj[i].base.list_used.cqe_next =
			get_role_obj_byidx(pool_role_obj[i].base.list_used.idx_next);
		if (!pool_role_obj[i].base.list_used.cqe_next)
			pool_role_obj[i].base.list_used.cqe_next = (ROLE *)pool_role_obj_used;
		
		pool_role_obj[i].base.list_used.cqe_prev =
			get_role_obj_byidx(pool_role_obj[i].base.list_used.idx_prev);
		if (!pool_role_obj[i].base.list_used.cqe_prev)
			pool_role_obj[i].base.list_used.cqe_prev = (ROLE *)pool_role_obj_used;
	}

	pool_role_obj_freed->cqh_first = get_role_obj_byidx(pool_role_obj_freed->idx_first);
	if (!pool_role_obj_freed->cqh_first)
		pool_role_obj_freed->cqh_first = (ROLE *)pool_role_obj_freed;
	pool_role_obj_freed->cqh_last = get_role_obj_byidx(pool_role_obj_freed->idx_last);
	if (!pool_role_obj_freed->cqh_last)
		pool_role_obj_freed->cqh_last = (ROLE *)pool_role_obj_freed;

	pool_role_obj_used->cqh_first = get_role_obj_byidx(pool_role_obj_used->idx_first);
	if (!pool_role_obj_used->cqh_first)
		pool_role_obj_used->cqh_first = (ROLE *)pool_role_obj_used;
	pool_role_obj_used->cqh_last = get_role_obj_byidx(pool_role_obj_used->idx_last);
	if (!pool_role_obj_used->cqh_last)
		pool_role_obj_used->cqh_last = (ROLE *)pool_role_obj_used;	
	return (0);
}

void init_scene_obj()
{
	int i;
	GAME_CIRCLEQ_INIT(pool_scene_obj_used);
	GAME_CIRCLEQ_INIT(pool_scene_obj_freed);
	for (i = 0; i < MAX_SCENE_OBJ_IN_GAME; ++i) {
		pool_scene_obj[i].base.list_free.idx = i;
		pool_scene_obj[i].base.list_used.idx = i;				
		GAME_CIRCLEQ_INSERT_HEAD(pool_scene_obj_freed, &pool_scene_obj[i], base.list_free);
	}	
}

SCENE *create_scene_obj()
{
	SCENE *first = pool_scene_obj_freed->cqh_first;
	if (!first || first == (SCENE *)pool_scene_obj_freed)
		return NULL;	
	GAME_CIRCLEQ_REMOVE(pool_scene_obj_freed, first, base.list_free);
	GAME_CIRCLEQ_INSERT_HEAD(pool_scene_obj_used, first, base.list_used);
	return first;	
}
void destory_scene_obj(SCENE *p)
{
	assert(p);
	GAME_CIRCLEQ_REMOVE(pool_scene_obj_used, p, base.list_used);
	GAME_CIRCLEQ_INSERT_HEAD(pool_scene_obj_freed, p, base.list_free);		
}


static inline SCENE *get_scene_obj_byidx(uint32_t idx)
{
	if (idx < MAX_SCENE_OBJ_IN_GAME)
		return &pool_scene_obj[idx];
	return NULL;
}


int resume_scene_obj(uint32_t idx)
{
	int i;
	for (i = 0; i < MAX_SCENE_OBJ_IN_GAME; ++i) {
		assert(pool_scene_obj[i].base.list_free.idx == i);
		assert(pool_scene_obj[i].base.list_used.idx == i);
		pool_scene_obj[i].base.list_free.cqe_next =
			get_scene_obj_byidx(pool_scene_obj[i].base.list_free.idx_next);
		if (!pool_scene_obj[i].base.list_free.cqe_next)
			pool_scene_obj[i].base.list_free.cqe_next = (SCENE *)pool_scene_obj_freed;
		
		pool_scene_obj[i].base.list_free.cqe_prev =
			get_scene_obj_byidx(pool_scene_obj[i].base.list_free.idx_prev);;
		if (!pool_scene_obj[i].base.list_free.cqe_prev)
			pool_scene_obj[i].base.list_free.cqe_prev = (SCENE *)pool_scene_obj_freed;
		
		pool_scene_obj[i].base.list_used.cqe_next =
			get_scene_obj_byidx(pool_scene_obj[i].base.list_used.idx_next);
		if (!pool_scene_obj[i].base.list_used.cqe_next)
			pool_scene_obj[i].base.list_used.cqe_next = (SCENE *)pool_scene_obj_used;
		
		pool_scene_obj[i].base.list_used.cqe_prev =
			get_scene_obj_byidx(pool_scene_obj[i].base.list_used.idx_prev);
		if (!pool_scene_obj[i].base.list_used.cqe_prev)
			pool_scene_obj[i].base.list_used.cqe_prev = (SCENE *)pool_scene_obj_used;
	}

	pool_scene_obj_freed->cqh_first = get_scene_obj_byidx(pool_scene_obj_freed->idx_first);
	if (!pool_scene_obj_freed->cqh_first)
		pool_scene_obj_freed->cqh_first = (SCENE *)pool_scene_obj_freed;
	pool_scene_obj_freed->cqh_last = get_scene_obj_byidx(pool_scene_obj_freed->idx_last);
	if (!pool_scene_obj_freed->cqh_last)
		pool_scene_obj_freed->cqh_last = (SCENE *)pool_scene_obj_freed;

	pool_scene_obj_used->cqh_first = get_scene_obj_byidx(pool_scene_obj_used->idx_first);
	if (!pool_scene_obj_used->cqh_first)
		pool_scene_obj_used->cqh_first = (SCENE *)pool_scene_obj_used;
	pool_scene_obj_used->cqh_last = get_scene_obj_byidx(pool_scene_obj_used->idx_last);
	if (!pool_scene_obj_used->cqh_last)
		pool_scene_obj_used->cqh_last = (SCENE *)pool_scene_obj_used;	
	return (0);
}

void init_instance_obj()
{
	int i;
	GAME_CIRCLEQ_INIT(pool_instance_obj_used);
	GAME_CIRCLEQ_INIT(pool_instance_obj_freed);
	for (i = 0; i < MAX_INSTANCE_OBJ_IN_GAME; ++i) {
		pool_instance_obj[i].base.list_free.idx = i;
		pool_instance_obj[i].base.list_used.idx = i;				
		GAME_CIRCLEQ_INSERT_HEAD(pool_instance_obj_freed, &pool_instance_obj[i], base.list_free);
	}	
}

INSTANCE *create_instance_obj()
{
	INSTANCE *first = pool_instance_obj_freed->cqh_first;
	if (!first || first == (INSTANCE *)pool_instance_obj_freed)
		return NULL;	
	GAME_CIRCLEQ_REMOVE(pool_instance_obj_freed, first, base.list_free);
	GAME_CIRCLEQ_INSERT_HEAD(pool_instance_obj_used, first, base.list_used);
	return first;	
}
void destory_instance_obj(INSTANCE *p)
{
	assert(p);
	GAME_CIRCLEQ_REMOVE(pool_instance_obj_used, p, base.list_used);
	GAME_CIRCLEQ_INSERT_HEAD(pool_instance_obj_freed, p, base.list_free);		
}


static inline INSTANCE *get_instance_obj_byidx(uint32_t idx)
{
	if (idx < MAX_INSTANCE_OBJ_IN_GAME)
		return &pool_instance_obj[idx];
	return NULL;
}


int resume_instance_obj(uint32_t idx)
{
	int i;
	for (i = 0; i < MAX_INSTANCE_OBJ_IN_GAME; ++i) {
		assert(pool_instance_obj[i].base.list_free.idx == i);
		assert(pool_instance_obj[i].base.list_used.idx == i);
		pool_instance_obj[i].base.list_free.cqe_next =
			get_instance_obj_byidx(pool_instance_obj[i].base.list_free.idx_next);
		if (!pool_instance_obj[i].base.list_free.cqe_next)
			pool_instance_obj[i].base.list_free.cqe_next = (INSTANCE *)pool_instance_obj_freed;
		
		pool_instance_obj[i].base.list_free.cqe_prev =
			get_instance_obj_byidx(pool_instance_obj[i].base.list_free.idx_prev);;
		if (!pool_instance_obj[i].base.list_free.cqe_prev)
			pool_instance_obj[i].base.list_free.cqe_prev = (INSTANCE *)pool_instance_obj_freed;
		
		pool_instance_obj[i].base.list_used.cqe_next =
			get_instance_obj_byidx(pool_instance_obj[i].base.list_used.idx_next);
		if (!pool_instance_obj[i].base.list_used.cqe_next)
			pool_instance_obj[i].base.list_used.cqe_next = (INSTANCE *)pool_instance_obj_used;
		
		pool_instance_obj[i].base.list_used.cqe_prev =
			get_instance_obj_byidx(pool_instance_obj[i].base.list_used.idx_prev);
		if (!pool_instance_obj[i].base.list_used.cqe_prev)
			pool_instance_obj[i].base.list_used.cqe_prev = (INSTANCE *)pool_instance_obj_used;
	}

	pool_instance_obj_freed->cqh_first = get_instance_obj_byidx(pool_instance_obj_freed->idx_first);
	if (!pool_instance_obj_freed->cqh_first)
		pool_instance_obj_freed->cqh_first = (INSTANCE *)pool_instance_obj_freed;
	pool_instance_obj_freed->cqh_last = get_instance_obj_byidx(pool_instance_obj_freed->idx_last);
	if (!pool_instance_obj_freed->cqh_last)
		pool_instance_obj_freed->cqh_last = (INSTANCE *)pool_instance_obj_freed;

	pool_instance_obj_used->cqh_first = get_instance_obj_byidx(pool_instance_obj_used->idx_first);
	if (!pool_instance_obj_used->cqh_first)
		pool_instance_obj_used->cqh_first = (INSTANCE *)pool_instance_obj_used;
	pool_instance_obj_used->cqh_last = get_instance_obj_byidx(pool_instance_obj_used->idx_last);
	if (!pool_instance_obj_used->cqh_last)
		pool_instance_obj_used->cqh_last = (INSTANCE *)pool_instance_obj_used;	
	return (0);
}
#endif
//////////////////////////////////////////////////////////
/*
static int count_mem_used()
{
	int ret = 0;
	ret += sizeof(struct pool_container_head) * 2 + sizeof(CONTAINER) * max_container_in_game;
	ret += sizeof(struct pool_thing_obj_head) * 2 + sizeof(THING_OBJ) * max_thing_obj_in_game;
	ret += sizeof(struct pool_role_head) * 2 + sizeof(ROLE) * max_role_in_game;
	ret += sizeof(struct pool_scene_head) * 2 + sizeof(SCENE) * max_scene_in_game;
	ret += sizeof(struct pool_instance_head) * 2 + sizeof(INSTANCE) * max_instance_in_game;		
	return (ret);
}

int alloc_shared_mem(int resume)
{
	int size = count_mem_used();
	int shmid;
	if (resume)
		shmid = shmget(key, size, 0666);
	else
		shmid = shmget(key, size, IPC_CREAT|IPC_EXCL|0666);
	if (shmid < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: shmget resume[%d] failed[%d]", __FUNCTION__, __LINE__, resume, errno);
		return (-1);
	}
	
	void *mem = shmat(shmid, NULL, 0);
	if (!mem) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: shmat resume[%d] failed[%d]", __FUNCTION__, __LINE__, resume, errno);
		return (-10);
	}
	int offset = 0;
	pool_container_used = mem + offset;
	offset += sizeof(*pool_container_used);
	
	pool_container_freed = mem + offset;
	offset += sizeof(*pool_container_freed);
	
	pool_container = mem + offset;
	offset += sizeof(CONTAINER) * max_container_in_game;

	pool_thing_obj_used = mem + offset;
	offset += sizeof(*pool_thing_obj_used);

	pool_thing_obj_freed = mem + offset;
	offset += sizeof(*pool_thing_obj_freed);

	pool_thing_obj = mem + offset;
	offset += sizeof(THING_OBJ) * max_thing_obj_in_game;

	pool_role_used = mem + offset;
	offset += sizeof(*pool_role_used);

	pool_role_freed = mem + offset;
	offset += sizeof(*pool_role_freed);

	pool_role = mem + offset;
	offset += sizeof(ROLE) * max_role_in_game;

	pool_scene_used = mem + offset;
	offset += sizeof(*pool_scene_used);

	pool_scene_freed = mem + offset;
	offset += sizeof(*pool_scene_freed);

	pool_scene = mem + offset;
	offset += sizeof(SCENE) * max_scene_in_game;		

	pool_instance_used = mem + offset;
	offset += sizeof(*pool_instance_used);

	pool_instance_freed = mem + offset;
	offset += sizeof(*pool_instance_freed);

	pool_instance = mem + offset;
	offset += sizeof(INSTANCE) * max_instance_in_game;

	return (0);
}
*/


