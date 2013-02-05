#ifndef _SERVER_SPECIAL_PROTO_H__
#define _SERVER_SPECIAL_PROTO_H__

#include "server_proto.h" 
#include "db_role_info.pb-c.h"

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

typedef struct db_thing_info
{
	uint32_t id;    //物品ID
	uint16_t num;   //数量为0的时候删除
	uint8_t pos;
} DB_THING_INFO;

typedef struct db_quest_info
{
	uint32_t id;
	uint32_t state;
	uint32_t accept_time;
	uint32_t cycle_num;
	uint32_t kill_npc1_num;
	uint32_t kill_npc2_num;
	uint32_t kill_npc3_num;
	uint32_t collect_item1_num;
	uint32_t collect_item2_num;
	uint32_t collect_item3_num;
} DB_QUEST_INFO;

typedef struct db_role_info
{
	char name[MAX_ROLE_NAME_LEN];                  //角色名
	uint8_t profession;                            //职业  不可变属性
	uint16_t level;                                //等级  可变属性
//	int32_t hp_cur;                                //当前血量  可变属性
	uint16_t scene_id;                             //场景ID
	POS pos;                                       //位置
//	uint32_t active_skill[MAX_ACTIVE_SKILL];       //设置要释放的技能
//	uint32_t skill[MAX_SKILL_PER_ROLE];            //所学的技能
	uint32_t money;
	uint32_t gold;	
	uint16_t blob_size;
	uint8_t blob_info[0];
} DB_ROLE_INFO;

#define QUEST_OFFSET(ptr) (offsetof(DB_ROLE_INFO, bag_things) + bag_size)
#define QUEST_PTR(ptr) (((char *)(ptr)) + QUEST_OFFSET((ptr)))

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
