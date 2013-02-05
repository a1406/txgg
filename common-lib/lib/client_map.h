#ifndef _CONNSRV_CLIENT_MAP_H__
#define _CONNSRV_CLIENT_MAP_H__

#include <stdint.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include "game_queue.h"
#include "mempool.h"
#include "rbtree.h"

#define MAX_BUF_PER_CLIENT (64 * 1024 - 1)

struct client_map_struct;
typedef int (*cb_recv_func)(evutil_socket_t fd);
typedef void (*cb_release_client_map)(struct client_map_struct *client_map);

typedef struct client_map_struct
{
	OBJ_BASE(client_map_struct) base;
	uint16_t fd;
	struct rb_node node_fd;		
	cb_recv_func cb_recv;
	cb_release_client_map cb_release;
	struct event event_recv;		
	uint8_t server_id;
	uint16_t pos_begin;	
	uint16_t pos_end;	
	uint8_t buf[MAX_BUF_PER_CLIENT];
} CLIENT_MAP;

void init_client_map_data(CLIENT_MAP *client);
int resume_client_map_data(CLIENT_MAP *client);

#define CLIENT_MAP_BUF_SIZE(client) (client->pos_end - client->pos_begin)
#define CLIENT_MAP_BUF_HEAD(client) (client->buf + client->pos_begin)
#define CLIENT_MAP_BUF_TAIL(client) (client->buf + client->pos_end)
#define CLIENT_MAP_BUF_LEAVE(client) (MAX_BUF_PER_CLIENT - client->pos_end)

CLIENT_MAP *get_first_client_map(struct rb_root *head);
CLIENT_MAP *get_next_client_map(CLIENT_MAP *client, int left);

CLIENT_MAP *get_client_map_by_fd(uint16_t fd, struct rb_root *head);
CLIENT_MAP *add_client_map(CLIENT_MAP *client, struct rb_root *head);
int del_client_map_by_fd(uint16_t fd, struct rb_root *head);

extern int max_client_map_in_game;
void init_game_client_map(int num);

POOL_DEC(client_map)

#endif
