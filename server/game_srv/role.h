#ifndef _GAMESRV_ROLE_H__
#define _GAMESRV_ROLE_H__

#include <time.h>
#include "path.h"
#include "game_queue.h"
#include "container.h"
#include "client_map.h"
#include "game_define.h"
#include "cs.pb-c.h"
#include "special_proto.h"
#include "rbtree.h"
#include "sortarray.h"
#include "one_role.h"

struct scene_struct;
struct instance_struct;

#define INSTANCE_STATE_FINISHED (1)
#define INSTANCE_STATE_INPROGRESS (0)

struct instance_data
{
	uint8_t state;                       //状态，进行中（0），完成（1）
	uint8_t action;                      //在副本中要触发的事件，事件触发之后才能进入下一步
	uint16_t seq_id;                     //在副本中的位置下标
	uint8_t dice;                        //骰子的数字
	uint8_t special_dice;                //指定的骰子数
	uint8_t nowar_times;                 //免战次数
	uint8_t back_steps;                  //需要倒着走的格子数
	uint8_t lucky_drop;                  //幸运格增加的掉率
	uint8_t lucky_money;                 //幸运格增加的money获得比率
	uint8_t lucky_exp;		             //幸运格增加的经验获得比率
};

struct shortcut
{
	uint32_t id;
	uint8_t pos;
};

struct game_friend_data
{
	uint64_t friend[MAX_GAME_FRIEND_PER_ROLE];
	uint64_t blacklist[MAX_GAME_FRIEND_PER_ROLE];
};

typedef struct role_struct
{
	OBJ_BASE(role_struct) base;
	struct rb_node node_id;
	struct rb_node node_fd;	
	struct rb_node node_name;

//	uint8_t conn_srv_index;                          //连接服务器's FD
	uint32_t session_id;                                   //连接服务器与之通讯的FD  低16位表示FD，中间8位表示连接服务器的index，高8位预留
	uint32_t money;
	uint32_t gold;		
	uint64_t role_id;                              //帐号ID
	uint32_t move_speed;                           //移动速度	
	PATH path;                                     //行走路径
	struct timeval last_refresh_time;              //行走路径最近更新时间
	IDX scene;                                     //所在场景
	IDX instance;                                 //所在副本
	struct instance_data instance_data;
	IDX bag;                                       //背包
	IDX store;                                     //仓库
	uint8_t quest_num;
	DB_QUEST_INFO quest[MAX_QUEST_PER_ROLE];       //任务
	uint32_t main_quest_index;
	uint16_t finished_slave_quest_num;	
	uint32_t all_finished_slave_quest[MAX_SLAVE_QUEST];
	struct shortcut shortcut[MAX_SHORTCUT_PER_ROLE];
	struct game_friend_data game_friend;	
	ONE_ROLE one_role[MAX_FRIEND_PER_ROLE];       //0是主角，1,2,是出战宝宝，其他是未出战宝宝
} ROLE, *handle_role;

#define CONN_INDEX(role) (((role->session_id) >> 16) & 0xff)
#define ROLE_FD(role) (role->session_id & 0xffff)

void reset_role_attribute(ROLE *role);

void init_role_data(ROLE *role);
int resume_role_data(ROLE *role);

int add_role_quest(ROLE *role, DB_QUEST_INFO *quest);
int remove_role_quest(ROLE *role, uint32_t id);
DB_QUEST_INFO *get_role_quest(ROLE *role, uint32_t id);

uint16_t get_role_sceneid(ROLE *role);

inline uint8_t is_role_friend_exist(ROLE *role, uint8_t pos);
inline int give_role_exp(ROLE *role, uint32_t exp);
inline int give_role_drop(ROLE *role, uint32_t drop_id, DropData *data);
inline int give_role_thing_list(ROLE *role, struct thing_list *list);
inline int modify_role_money(ROLE *role, int money);
inline int modify_role_gold(ROLE *role, int gold);

inline int give_role_thing_bypos(ROLE *role, uint8_t pos, uint32_t id, uint16_t num);
inline int delete_role_thing_bypos(ROLE *role, uint8_t pos, uint16_t num);
inline int swap_role_thing_pos(ROLE *role, uint8_t pos1, uint8_t pos2);

int add_one_role(ROLE *role);
int del_one_role(ROLE *role);

void set_role_scene(ROLE *role, struct scene_struct *scene);
void set_role_bag(ROLE *role, CONTAINER *bag);
void set_role_instance(ROLE *role, struct instance_struct *instance);
void set_role_store(ROLE *role, CONTAINER *store);

void dump_role_sight_info(ROLE *role, RoleSightInfo *info);

void send_store_role_request(ROLE *role, CLIENT_MAP *roledb);

//不需要返回了，如果失败直接踢线，记日志
inline void role_login(ROLE *role);
inline int role_logout(ROLE *role);
inline int kick_role(ROLE *role, uint32_t reason);

void role_rbdata_init_head();

ROLE *get_role_by_session(uint32_t session);
ROLE *get_role_by_id(uint64_t id);
ROLE *get_role_by_name(char *name);
void test_add_role();

int send_to_role(ROLE *role, PROTO_HEAD *head);


int role_modify_hp(ROLE *role, int hp);
int role_modify_lucky(ROLE *role, int drop, int money, int exp);

inline uint32_t get_role_equip_id(ROLE *role, int friend_pos, int equip_pos);
inline int set_role_equip_id(ROLE *role, int friend_pos, int equip_pos, uint32_t new_equip_id);

inline int send_attr_changed_notify(ROLE *role, int id, int new_value);

int add_role_game_friend(ROLE *role, uint64_t id);
int del_role_game_friend(ROLE *role, uint64_t id);
int add_role_blacklist(ROLE *role, uint64_t id);
int del_role_blacklist(ROLE *role, uint64_t id);
int get_role_game_friend_num(ROLE *role);
int get_role_game_blacklist_num(ROLE *role);
	
#endif
