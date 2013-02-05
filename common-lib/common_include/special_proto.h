#ifndef _SERVER_SPECIAL_PROTO_H__
#define _SERVER_SPECIAL_PROTO_H__

#include "server_proto.h" 

typedef struct register_server_request
{
	uint8_t id;
} REGISTER_SERVER_REQUEST;

typedef struct create_role_request
{
	uint64_t id;
	char name[MAX_ROLE_NAME_LEN];
	uint8_t profession;
} CREATE_ROLE_REQUEST;

typedef struct db_role_info
{
	char name[MAX_ROLE_NAME_LEN];                  //角色名
	uint8_t profession;                            //职业  不可变属性
	uint16_t level;                                //等级  可变属性
	int32_t hp_cur;                                //当前血量  可变属性
	int32_t hp_max;                                //最大血量  可变属性
	uint16_t scene_id;                             //场景ID
	POS pos;                                       //位置
	uint32_t active_skill[MAX_ACTIVE_SKILL];       //设置要释放的技能
	uint32_t skill[MAX_SKILL_PER_ROLE];            //所学的技能
//	CONTAINER bag;
} DB_ROLE_INFO;

typedef struct create_role_response
{
	uint32_t result;
	uint64_t id;
	DB_ROLE_INFO info;	
} CREATE_ROLE_RESPONSE;

typedef struct logout_request
{
	uint32_t reason;
} LOGOUT_REQUEST;

typedef struct logout_response
{
	uint64_t id;
	int32_t result;
} LOGOUT_RESPONSE;

//踢人
typedef struct kick_notify
{
	uint32_t reason;
} KICK_NOTIFY;

typedef struct fetch_role_data_request
{
	uint64_t id;
} FETCH_ROLE_DATA_REQUEST;

typedef struct fetch_role_data_response
{
	uint32_t result;	
	uint64_t id;                                   //帐号ID
	DB_ROLE_INFO info;
} FETCH_ROLE_DATA_RESPONSE;

typedef struct store_role_data_request
{
	uint64_t id;
	DB_ROLE_INFO info;	
} STORE_ROLE_DATA_REQUEST;

typedef struct store_role_data_response
{
	uint32_t result;	
	uint64_t id;                                   //帐号ID
} STORE_ROLE_DATA_RESPONSE;

#endif
