#ifndef _GAME_PATH_H__
#define _GAME_PATH_H__

#include <stdint.h>
#include <sys/time.h>
#include "server_proto.h"

int refresh_path(PATH *path, struct timeval *old, struct timeval *now, uint16_t speed);

#endif
