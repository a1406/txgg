#ifndef _CONNSRV_EVENT_H__
#define _CONNSRV_EVENT_H__

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include "log4c.h"
#include "client_map.h"

typedef int (*cb_listen_pre_func)();
typedef int (*cb_listen_after_func)(evutil_socket_t fd);
typedef CLIENT_MAP *(*cb_get_client_map)(evutil_socket_t fd);

typedef struct listen_callbacks
{
//	uint8_t need_free;
//	cb_recv_func cb_recv;
	cb_listen_pre_func cb_listen_pre;
	cb_listen_after_func cb_listen_after;
	cb_get_client_map cb_get_client_map;
	struct event event_accept;	
} LISTEN_CALLBACKS;

extern log4c_category_t* mycat;
extern struct event_base *base;
extern cb_recv_func game_cb_recv;
extern cb_listen_pre_func game_cb_listen_pre;
extern cb_listen_after_func game_cb_listen_after;

int game_event_init();
int game_add_listen_event(uint16_t port, LISTEN_CALLBACKS *callback);
int game_add_connect_event(struct sockaddr *sa, int socklen, CLIENT_MAP *client_map);
int add_timer(struct timeval t, struct event *event_timer);
int add_signal(int signum, struct event *event);
void remove_listen_callback_event(CLIENT_MAP *client_map);
int game_set_socket_opt(int fd);
#endif

