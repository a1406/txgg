#ifndef _SORTARRAY_H__
#define _SORTARRAY_H__
#include <stdint.h>

typedef int (*comp_func)(const void *, const void *);

int array_bsearch (const void *key, const void *base, int nmemb, int size, int *find, comp_func func);
int array_delete_index (const void *base, int *pnmemb, int size, int index);
int array_delete (const void *key, const void *base, int *pnmemb, int size, comp_func func);
int array_insert (const void *key, const void *base, int *pnmemb, int size, int unique, comp_func func);

int comp_int64(const void *a, const void *b);
int comp_uint64(const void *a, const void *b);
int comp_int32(const void *a, const void *b);
int comp_uint32(const void *a, const void *b);	
#endif

