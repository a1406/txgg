#include <stdlib.h>
#include <assert.h>
#include "game_rand.h"

const int MAX_RANDOM_NUMBER = 32767;

uint32_t get_rand_between(uint32_t min, uint32_t max)
{
    uint32_t i = 0;
    uint64_t tmp = 0;

    assert(max >= min);
    
    if (min == max)
    {
        return min;
    }
    else if (max <= MAX_RANDOM_NUMBER)
    {
        i = random() % (max - min + 1) + min;
    }
    else
    {
        tmp = (max + MAX_RANDOM_NUMBER - 1) * random();
        i = abs(tmp % (max - min));
        i += min;
    }

    assert(i >= min);
    assert(i <= max);

    return i;
}
