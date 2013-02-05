#ifndef _GAME_QUEUE_H__
#define _GAME_QUEUE_H__

#include <stdint.h>
#include <stddef.h>
#include "idx.h"

#ifndef container_of
#define container_of(ptr, type, member) ({            \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#ifndef likely
#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)
#endif

/*
 * Circular queue definitions.
 */
#define	GAME_CIRCLEQ_HEAD(name, type)					\
struct name {								\
	struct type *cqh_first;		/* first element */		\
	uint32_t idx_first;									\
	struct type *cqh_last;		/* last element */		\
	uint32_t idx_last;									\
}

#define	GAME_CIRCLEQ_HEAD_INITIALIZER(head)					\
	{ (void *)&head, MAX_IDX, (void *)&head, MAX_IDX }

#define	GAME_CIRCLEQ_ENTRY(type)						\
struct {								\
	uint32_t idx;										\
	struct type *cqe_next;		/* next element */		\
	uint32_t idx_next;									\
	struct type *cqe_prev;		/* previous element */		\
	uint32_t idx_prev;									\
}

/*
 * Circular queue functions.
 */
#define	GAME_CIRCLEQ_INIT(head) do {						\
	(head)->cqh_first = (void *)(head);				\
	(head)->idx_first = MAX_IDX;				\
	(head)->cqh_last = (void *)(head);				\
	(head)->idx_last = MAX_IDX;				\
} while (/*CONSTCOND*/0)

#define	GAME_CIRCLEQ_INSERT_AFTER(head, listelm, elm, field) do {		\
	(elm)->field.cqe_next = (listelm)->field.cqe_next;		\
	(elm)->field.idx_next = (listelm)->field.idx_next;		\
	(elm)->field.cqe_prev = (listelm);				\
	(elm)->field.idx_prev = (listelm)->field.idx;				\
	if ((void *)(listelm)->field.cqe_next == (void *)(head)) {		\
		(head)->cqh_last = (elm);				\
		(head)->idx_last = (elm)->field.idx;	\
	}											\
	else {													\
		(listelm)->field.cqe_next->field.cqe_prev = (elm);	\
    	(listelm)->field.cqe_next->field.idx_prev = (elm)->field.idx;	\
	}														\
	(listelm)->field.cqe_next = (elm);				\
	(listelm)->field.idx_next = (elm)->field.idx;	\
} while (/*CONSTCOND*/0)

#define	GAME_CIRCLEQ_INSERT_BEFORE(head, listelm, elm, field) do {		\
	(elm)->field.cqe_next = (listelm);				\
	(elm)->field.idx_next = (listelm)->field.idx;				\
	(elm)->field.cqe_prev = (listelm)->field.cqe_prev;		\
	(elm)->field.idx_prev = (listelm)->field.idx_prev;		\
	if ((void *)(listelm)->field.cqe_prev == (void *)(head)) {	\
		(head)->cqh_first = (elm);				\
		(head)->idx_first = (elm)->field.idx;				\
	}											\
	else {													\
		(listelm)->field.cqe_prev->field.cqe_next = (elm);	\
		(listelm)->field.cqe_prev->field.idx_next = (elm)->field.idx;	\
	}														\
	(listelm)->field.cqe_prev = (elm);				\
	(listelm)->field.idx_prev = (elm)->field.idx;				\
} while (/*CONSTCOND*/0)

#define	GAME_CIRCLEQ_INSERT_HEAD(head, elm, field) do {			\
	(elm)->field.cqe_next = (head)->cqh_first;			\
	(elm)->field.idx_next = (head)->idx_first;			\
	(elm)->field.cqe_prev = (void *)(head);				\
	(elm)->field.idx_prev = MAX_IDX;				\
	if ((head)->cqh_last == (void *)(head)) {	\
		(head)->cqh_last = (elm);				\
		(head)->idx_last = (elm)->field.idx;				\
	}											\
	else {												\
		(head)->cqh_first->field.cqe_prev = (elm);		\
		(head)->cqh_first->field.idx_prev = (elm)->field.idx;		\
	}													\
	(head)->cqh_first = (elm);					\
	(head)->idx_first = (elm)->field.idx;					\
} while (/*CONSTCOND*/0)

#define	GAME_CIRCLEQ_INSERT_TAIL(head, elm, field) do {			\
	(elm)->field.cqe_next = (void *)(head);				\
	(elm)->field.idx_next = MAX_IDX;				\
	(elm)->field.cqe_prev = (head)->cqh_last;			\
	(elm)->field.idx_prev = (head)->idx_last;			\
	if ((head)->cqh_first == (void *)(head)) {			\
		(head)->cqh_first = (elm);				\
		(head)->idx_first = (elm).field.idx;				\
	}											\
	else {												\
		(head)->cqh_last->field.cqe_next = (elm);		\
		(head)->cqh_last->field.idx_next = (elm).field.idx;		\
	}													\
	(head)->cqh_last = (elm);					\
	(head)->cqh_last = (elm).field.idx;					\
} while (/*CONSTCOND*/0)

#define	GAME_CIRCLEQ_REMOVE(head, elm, field) do {				\
	if ((void *)(elm)->field.cqe_next == (void *)(head)) {	\
		(head)->cqh_last = (elm)->field.cqe_prev;		\
    	(head)->idx_last = (elm)->field.idx_prev;		\
	}									\
	else {												\
		(elm)->field.cqe_next->field.cqe_prev =			\
		    (elm)->field.cqe_prev;				\
		(elm)->field.cqe_next->field.idx_prev =			\
		    (elm)->field.idx_prev;				\
	}														\
	if ((void *)(elm)->field.cqe_prev == (void *)(head)) {		\
		(head)->cqh_first = (elm)->field.cqe_next;		\
		(head)->idx_first = (elm)->field.idx_next;		\
	}													\
	else {												\
		(elm)->field.cqe_prev->field.cqe_next =			\
		    (elm)->field.cqe_next;				\
		(elm)->field.cqe_prev->field.idx_next =			\
		    (elm)->field.idx_next;				\
	}											\
} while (/*CONSTCOND*/0)

#define	GAME_CIRCLEQ_FOREACH(var, head, field)				\
	for ((var) = ((head)->cqh_first);				\
		(var) != (const void *)(head);				\
		(var) = ((var)->field.cqe_next))

#define	GAME_CIRCLEQ_FOREACH_REVERSE(var, head, field)			\
	for ((var) = ((head)->cqh_last);				\
		(var) != (const void *)(head);				\
		(var) = ((var)->field.cqe_prev))

/*
 * Circular queue access methods.
 */
#define	GAME_CIRCLEQ_EMPTY(head)		((void *)(head)->cqh_first == (void *)(head))
#define	GAME_CIRCLEQ_FIRST(head)		((head)->cqh_first)
#define	GAME_CIRCLEQ_LAST(head)		((head)->cqh_last)
#define	GAME_CIRCLEQ_NEXT(elm, field)	((elm)->field.cqe_next)
#define	GAME_CIRCLEQ_PREV(elm, field)	((elm)->field.cqe_prev)

#define GAME_CIRCLEQ_LOOP_NEXT(head, elm, field)				\
	(((void *)(elm)->field.cqe_next == (void *)(head))			\
	    ? ((head)->cqh_first)					\
	    : (elm->field.cqe_next))
#define GAME_CIRCLEQ_LOOP_PREV(head, elm, field)				\
	(((void *)(elm)->field.cqe_prev == (void *)(head))			\
	    ? ((head)->cqh_last)					\
	    : (elm->field.cqe_prev))


#ifdef DEBUG_MEM_POOL
#define OBJ_BASE(type)  \
struct {      \
	GAME_CIRCLEQ_ENTRY(type) list_free;   \
	GAME_CIRCLEQ_ENTRY(type) list_used;	  \
	uint8_t inuse;					  \
}
#else
#define OBJ_BASE(type)  \
struct {      \
	GAME_CIRCLEQ_ENTRY(type) list_free;   \
	GAME_CIRCLEQ_ENTRY(type) list_used;	  \
}

#endif				    			 



#define GET_IDX(obj)   (obj->base.list_free.idx)

#endif
