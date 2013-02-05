#ifndef _GAMESRV_IDX_H__
#define _GAMESRV_IDX_H__

#include <stdint.h>

#define MAX_IDX  (0xffffffff)

typedef struct idx_struct
{
	void *p;
	uint32_t idx;
} IDX;

inline int comp_idx(const void *, const void *);

#endif
