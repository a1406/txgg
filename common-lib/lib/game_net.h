#ifndef _GAME_NET_H__
#define _GAME_NET_H__
#include "event2/util.h"
#include "client_map.h"
#include "server_proto.h"

#ifdef CALC_NET_MSG
#define   CS__MESSAGE__ID__MAX_MSG_ID  (2048)
extern uint32_t send_buf_times[CS__MESSAGE__ID__MAX_MSG_ID];
extern uint32_t recv_buf_times[CS__MESSAGE__ID__MAX_MSG_ID];
extern uint32_t send_buf_size[CS__MESSAGE__ID__MAX_MSG_ID];
extern uint32_t recv_buf_size[CS__MESSAGE__ID__MAX_MSG_ID];
#endif

//返回0表示接收完毕，返回大于0表示没接收完毕。返回小于零表示断开
int get_one_buf(evutil_socket_t fd, CLIENT_MAP *client);

//返回0表示正常，返回大于0表示还有包没有处理完, 不会小于0
int remove_one_buf(CLIENT_MAP *client);

int send_one_msg(evutil_socket_t fd, PROTO_HEAD *head, uint8_t force);
#endif
