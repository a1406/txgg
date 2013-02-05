#ifndef _GAMESRV_MEMPOOL_H__
#define _GAMESRV_MEMPOOL_H__

#include <sys/queue.h>
#include <stdint.h>
#include "log4c.h"

extern log4c_category_t* mycat;

/*
#include "idx.h"
#include "role.h"
#include "scene.h"
#include "thing_obj.h"
#include "container.h"
#include "role.h"
#include "scene.h"
#include "pinstance.h"
*/

/*
void init_container();
CONTAINER *create_container();
int resume_container();
void destory_container(CONTAINER* p);

void init_thing_obj();
THING_OBJ *create_thing_obj();
int resume_thing_obj();
void destory_thing_obj(THING_OBJ *p);

int alloc_shared_mem(int resume);
*/

/*
extern GAME_CIRCLEQ_HEAD(pool_container_head, container_struct) *pool_container_used, *pool_container_freed;
extern GAME_CIRCLEQ_HEAD(pool_thing_obj_head, thing_obj_struct) *pool_thing_obj_used, *pool_thing_obj_freed;

extern GAME_CIRCLEQ_HEAD(pool_role_obj_head, role_struct) *pool_role_used, *pool_role_freed;
extern GAME_CIRCLEQ_HEAD(pool_scene_obj_head, scene_struct) *pool_scene_used, *pool_scene_freed;

extern GAME_CIRCLEQ_HEAD(pool_pinstance_obj_head, pinstance_struct) *pool_pinstance_used, *pool_pinstance_freed;
*/

#define POOL_DEC(name)   \
		extern GAME_CIRCLEQ_HEAD(pool_##name##_head, name##_struct) *pool_##name##_used, *pool_##name##_freed; \
		extern struct name##_struct *pool_##name; \
		extern inline struct name##_struct *get_##name##_byidx(uint32_t idx);   \
		extern inline struct name##_struct *create_##name();   \
		extern inline void destory_##name(struct name##_struct *p);  \
		extern void init_##name();									 \
		extern int resume_##name();									


/*
POOL_DEC(container)
POOL_DEC(thing_obj)
POOL_DEC(role)
POOL_DEC(scene)
POOL_DEC(pinstance)
*/

#ifdef DEBUG_MEM_POOL
#define POOL_DEF(name)  											\
		struct pool_##name##_head *pool_##name##_used, *pool_##name##_freed;  \
		struct name##_struct *pool_##name; \
												  \
		void init_##name()   \
		{    \
			int i;   \
			GAME_CIRCLEQ_INIT(pool_##name##_used);	\
			GAME_CIRCLEQ_INIT(pool_##name##_freed);	  \
			for (i = 0; i < max_##name##_in_game; ++i) {	\
				pool_##name[i].base.list_free.idx = i;  \
				pool_##name[i].base.list_used.idx = i;				\
				GAME_CIRCLEQ_INSERT_HEAD(pool_##name##_freed, &pool_##name[i], base.list_free); \
            	pool_##name[i].base.inuse = 0;			  \
			}	 \
		}   \
			\
		inline struct name##_struct *create_##name()  \
		{  \
			struct name##_struct *first = pool_##name##_freed->cqh_first;  \
			if (!first || first == (struct name##_struct *)pool_##name##_freed)	\
				return NULL;											\
			GAME_CIRCLEQ_REMOVE(pool_##name##_freed, first, base.list_free); \
			GAME_CIRCLEQ_INSERT_HEAD(pool_##name##_used, first, base.list_used); \
			assert(first->base.inuse == 0);				  \
           	first->base.inuse = 1;  			  \
			init_##name##_data(first);									\
			return first;  \
		}   \
		inline void destory_##name(struct name##_struct *p)  \
		{   \
			assert(p);   \
			GAME_CIRCLEQ_REMOVE(pool_##name##_used, p, base.list_used);  \
			GAME_CIRCLEQ_INSERT_HEAD(pool_##name##_freed, p, base.list_free);	\
			assert(p->base.inuse == 1);				  \
           	p->base.inuse = 0;  			  \
		}   \
    \
		inline struct name##_struct *get_##name##_byidx(uint32_t idx)   \
		{    \
			if (idx < max_##name##_in_game) {	\
				return &pool_##name[idx];  \
			}							   \
			return NULL;   \
		}    \
			 \
		int get_##name##_used_num()				\
		{										\
			int ret = 0;						\
			struct name##_struct *tmp;			\
			GAME_CIRCLEQ_FOREACH(tmp, pool_##name##_used, base.list_used) {   \
				assert(tmp->base.inuse == 1);				  \
				++ret;    \
			}         \
			return ret;							\
		}										\
		int get_##name##_freed_num()				\
		{										\
			int ret = 0;						\
			struct name##_struct *tmp;			\
			GAME_CIRCLEQ_FOREACH(tmp, pool_##name##_freed, base.list_free) {   \
				assert(tmp->base.inuse == 0);				  \
				++ret;    \
			}         \
			return ret;							\
		}										\
												\
		int resume_##name()  \
		{       \
			struct name##_struct *tmp;							\
			int i;  \
			for (i = 0; i < max_##name##_in_game; ++i) {  \
				assert(pool_##name[i].base.list_free.idx == i);  \
				assert(pool_##name[i].base.list_used.idx == i);  \
				pool_##name[i].base.list_free.cqe_next =   \
					get_##name##_byidx(pool_##name[i].base.list_free.idx_next);  \
				if (!pool_##name[i].base.list_free.cqe_next)   \
					pool_##name[i].base.list_free.cqe_next = (struct name##_struct *)pool_##name##_freed;  \
		  \
				pool_##name[i].base.list_free.cqe_prev =  \
					get_##name##_byidx(pool_##name[i].base.list_free.idx_prev);  \
				if (!pool_##name[i].base.list_free.cqe_prev)  \
					pool_##name[i].base.list_free.cqe_prev = (struct name##_struct *)pool_##name##_freed;  \
		\
				pool_##name[i].base.list_used.cqe_next =   \
					get_##name##_byidx(pool_##name[i].base.list_used.idx_next);  \
				if (!pool_##name[i].base.list_used.cqe_next)  \
					pool_##name[i].base.list_used.cqe_next = (struct name##_struct *)pool_##name##_used;  \
		   \
				pool_##name[i].base.list_used.cqe_prev =   \
					get_##name##_byidx(pool_##name[i].base.list_used.idx_prev);  \
				if (!pool_##name[i].base.list_used.cqe_prev)  \
					pool_##name[i].base.list_used.cqe_prev = (struct name##_struct *)pool_##name##_used;  \
																		\
			}  \
   \
			pool_##name##_freed->cqh_first = get_##name##_byidx(pool_##name##_freed->idx_first);  \
			if (!pool_##name##_freed->cqh_first)  \
				pool_##name##_freed->cqh_first = (struct name##_struct *)pool_##name##_freed;  \
			pool_##name##_freed->cqh_last = get_##name##_byidx(pool_##name##_freed->idx_last);  \
			if (!pool_##name##_freed->cqh_last)  \
				pool_##name##_freed->cqh_last = (struct name##_struct *)pool_##name##_freed;  \
   \
			pool_##name##_used->cqh_first = get_##name##_byidx(pool_##name##_used->idx_first);  \
			if (!pool_##name##_used->cqh_first)  \
				pool_##name##_used->cqh_first = (struct name##_struct *)pool_##name##_used; \
			pool_##name##_used->cqh_last = get_##name##_byidx(pool_##name##_used->idx_last);  \
			if (!pool_##name##_used->cqh_last)  \
				pool_##name##_used->cqh_last = (struct name##_struct *)pool_##name##_used;	  \
																		\
			GAME_CIRCLEQ_FOREACH(tmp, pool_##name##_used, base.list_used) {   \
				if (resume_##name##_data(tmp) != 0)			\
					return (-1);										\
			}         \
			return (0);  \
		}   

#else
#define POOL_DEF(name)  											\
		struct pool_##name##_head *pool_##name##_used, *pool_##name##_freed;  \
		struct name##_struct *pool_##name; \
												  \
		void init_##name()   \
		{    \
			int i;   \
			GAME_CIRCLEQ_INIT(pool_##name##_used);	\
			GAME_CIRCLEQ_INIT(pool_##name##_freed);	  \
			for (i = 0; i < max_##name##_in_game; ++i) {	\
				pool_##name[i].base.list_free.idx = i;  \
				pool_##name[i].base.list_used.idx = i;				\
				GAME_CIRCLEQ_INSERT_HEAD(pool_##name##_freed, &pool_##name[i], base.list_free); \
			}	 \
		}   \
			\
		inline struct name##_struct *create_##name()  \
		{  \
			struct name##_struct *first = pool_##name##_freed->cqh_first;  \
			if (!first || first == (struct name##_struct *)pool_##name##_freed)	\
				return NULL;											\
			GAME_CIRCLEQ_REMOVE(pool_##name##_freed, first, base.list_free); \
			GAME_CIRCLEQ_INSERT_HEAD(pool_##name##_used, first, base.list_used); \
			init_##name##_data(first);									\
			return first;  \
		}   \
		inline void destory_##name(struct name##_struct *p)  \
		{   \
			assert(p);   \
			GAME_CIRCLEQ_REMOVE(pool_##name##_used, p, base.list_used);  \
			GAME_CIRCLEQ_INSERT_HEAD(pool_##name##_freed, p, base.list_free);	\
		}   \
    \
		inline struct name##_struct *get_##name##_byidx(uint32_t idx)   \
		{    \
			if (idx < max_##name##_in_game) {	\
				return &pool_##name[idx];  \
			}							   \
			return NULL;   \
		}    \
			 \
		int get_##name##_used_num()				\
		{										\
			int ret = 0;						\
			struct name##_struct *tmp;			\
			GAME_CIRCLEQ_FOREACH(tmp, pool_##name##_used, base.list_used) {   \
				++ret;    \
			}         \
			return ret;							\
		}										\
		int get_##name##_freed_num()				\
		{										\
			int ret = 0;						\
			struct name##_struct *tmp;			\
			GAME_CIRCLEQ_FOREACH(tmp, pool_##name##_freed, base.list_free) {   \
				++ret;    \
			}         \
			return ret;							\
		}										\
												\
		int resume_##name()  \
		{       \
			struct name##_struct *tmp;							\
			int i;  \
			for (i = 0; i < max_##name##_in_game; ++i) {  \
				assert(pool_##name[i].base.list_free.idx == i);  \
				assert(pool_##name[i].base.list_used.idx == i);  \
				pool_##name[i].base.list_free.cqe_next =   \
					get_##name##_byidx(pool_##name[i].base.list_free.idx_next);  \
				if (!pool_##name[i].base.list_free.cqe_next)   \
					pool_##name[i].base.list_free.cqe_next = (struct name##_struct *)pool_##name##_freed;  \
		  \
				pool_##name[i].base.list_free.cqe_prev =  \
					get_##name##_byidx(pool_##name[i].base.list_free.idx_prev);  \
				if (!pool_##name[i].base.list_free.cqe_prev)  \
					pool_##name[i].base.list_free.cqe_prev = (struct name##_struct *)pool_##name##_freed;  \
		\
				pool_##name[i].base.list_used.cqe_next =   \
					get_##name##_byidx(pool_##name[i].base.list_used.idx_next);  \
				if (!pool_##name[i].base.list_used.cqe_next)  \
					pool_##name[i].base.list_used.cqe_next = (struct name##_struct *)pool_##name##_used;  \
		   \
				pool_##name[i].base.list_used.cqe_prev =   \
					get_##name##_byidx(pool_##name[i].base.list_used.idx_prev);  \
				if (!pool_##name[i].base.list_used.cqe_prev)  \
					pool_##name[i].base.list_used.cqe_prev = (struct name##_struct *)pool_##name##_used;  \
																		\
			}  \
   \
			pool_##name##_freed->cqh_first = get_##name##_byidx(pool_##name##_freed->idx_first);  \
			if (!pool_##name##_freed->cqh_first)  \
				pool_##name##_freed->cqh_first = (struct name##_struct *)pool_##name##_freed;  \
			pool_##name##_freed->cqh_last = get_##name##_byidx(pool_##name##_freed->idx_last);  \
			if (!pool_##name##_freed->cqh_last)  \
				pool_##name##_freed->cqh_last = (struct name##_struct *)pool_##name##_freed;  \
   \
			pool_##name##_used->cqh_first = get_##name##_byidx(pool_##name##_used->idx_first);  \
			if (!pool_##name##_used->cqh_first)  \
				pool_##name##_used->cqh_first = (struct name##_struct *)pool_##name##_used; \
			pool_##name##_used->cqh_last = get_##name##_byidx(pool_##name##_used->idx_last);  \
			if (!pool_##name##_used->cqh_last)  \
				pool_##name##_used->cqh_last = (struct name##_struct *)pool_##name##_used;	  \
																		\
			GAME_CIRCLEQ_FOREACH(tmp, pool_##name##_used, base.list_used) {   \
				if (resume_##name##_data(tmp) != 0)			\
					return (-1);										\
			}         \
			return (0);  \
		}   
#endif
#endif

