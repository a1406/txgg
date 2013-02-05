#include <assert.h>
#include "path.h"
#include "event2/util.h"

static inline float fastsqrtq3(float x)
{
#define SQRT_MAGIC_F 0x5f3759df

    const float xhalf = 0.5f*x;

    union
    {
        float x;
        int i;
    } u;
    u.x = x;
    u.i = SQRT_MAGIC_F - (u.i >> 1); 
    return x * u.x * (1.5f - xhalf * u.x * u.x);
}

static inline int getdistance(const POS *begin, const POS *end)
{
	assert(begin);
	assert(end);
    int x = end->pos_x - begin->pos_x;
    int y = end->pos_y - begin->pos_y;

    return (int)(fastsqrtq3(x * x + y * y));
}

int refresh_path(PATH *path, struct timeval *old, struct timeval *now, uint16_t speed)
{
	assert(path);
	assert(old);
	assert(now);
	struct timeval diff;
	if (*(uint32_t *)&path->end == *(uint32_t *)&path->begin)
		return (0);
	evutil_timersub(old, now, &diff);
	if (diff.tv_sec <= 0)
		return (0);

	int path_distance = getdistance(&path->begin, &path->end);
	int move_distance = speed * diff.tv_sec + speed * diff.tv_usec / 1000000;

	if (move_distance >= path_distance) {
		*(uint32_t *)&path->begin = *(uint32_t *)&path->end;
		return (0);
	}
	
	float rate = move_distance / path_distance;

    int x = path->end.pos_x - path->begin.pos_x;
    int y = path->end.pos_y - path->begin.pos_y;
	
	path->begin.pos_x += x * rate;
	path->begin.pos_y += y * rate;	
	return (0);
}
