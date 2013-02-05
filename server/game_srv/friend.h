#ifndef _GAMESRV_FRIEND_H__
#define _GAMESRV_FRIEND_H__
#include "game_queue.h"
#include <stdint.h>
#include "idx.h"
#include "game_srv.h"
int handle_friend_add_request(uint32_t session, const uint8_t *data, uint16_t len);
int handle_friend_del_request(uint32_t session, const uint8_t *data, uint16_t len);
int handle_friend_add_blacklist_request(uint32_t session, const uint8_t *data, uint16_t len);
int handle_friend_del_blacklist_request(uint32_t session, const uint8_t *data, uint16_t len);


#endif

