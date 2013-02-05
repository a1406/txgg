#ifndef _SERVER_PROTO_H__
#define _SERVER_PROTO_H__

#include <stdint.h>
#include "game_define.h"

enum SERVER_PROTO
{
	SERVER_PROTO_BROADCAST = 1000,	
	SERVER_PROTO_REGISTER_SERVER_REQUEST,
	SERVER_PROTO_REGISTER_SERVER_RESPONSE,
	SERVER_PROTO_FETCH_ROLE_REQUEST,
	SERVER_PROTO_FETCH_ROLE_RESPONSE,
	SERVER_PROTO_CREATE_ROLE_REQUEST,
	SERVER_PROTO_CREATE_ROLE_RESPONSE,
	SERVER_PROTO_KICK_ROLE_NOTIFY,		//踢人
	SERVER_PROTO_STORE_ROLE_REQUEST,
	SERVER_PROTO_STORE_ROLE_RESPONSE,
};

typedef struct proto_head
{
	uint16_t len;
	uint16_t msg_id;
	char data[0];
} PROTO_HEAD;

typedef struct proto_head_conn
{
	uint16_t len;
	uint16_t msg_id;
	uint16_t fd;      //从连接服务器给游戏服务器的时候加上fd
	char data[0];
} PROTO_HEAD_CONN;

typedef struct proto_head_conn_broadcast
{
	uint16_t len;
	uint16_t msg_id;
	uint8_t num_fd;
	PROTO_HEAD real_head;
	uint16_t fds[0];
} PROTO_HEAD_CONN_BROADCAST;

typedef struct normal_response
{
	int32_t result;
} NORMAL_RESPONSE;


typedef struct pos_struct
{
	int16_t pos_x;
	int16_t pos_y;
} POS;

typedef struct path_struct
{
	POS begin;
	POS end;
} PATH;

#endif

