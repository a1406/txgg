#include <assert.h>
#include "idx.h"

int comp_idx(const void *a, const void *b)
{
	assert(a);
	assert(b);

	if (((const IDX *)a)->idx == ((const IDX *)b)->idx)
		return (0);
	else if (((const IDX *)a)->idx < ((const IDX *)b)->idx)
		return (-1);
	return (1);
}
