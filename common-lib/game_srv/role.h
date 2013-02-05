#ifndef _GAMESRV_ROLE_H__
#define _GAMESRV_ROLE_H__

#include <time.h>
#include "path.h"
#include "game_queue.h"
#include "container.h"
#include "game_define.h"
#include "cs.pb-c.h"

struct scene_struct;

typedef struct role_struct
{
	OBJ_BASE(role_struct) base;
	uint16_t fd;                                   //连接服务器与之通讯的FD
	uint64_t role_id;                              //帐号ID
	char name[MAX_ROLE_NAME_LEN];                  //角色名
	uint32_t exp;                                  //经验
	uint8_t profession;                            //职业  不可变属性
	uint16_t level;                                //等级  可变属性
	int32_t hp_max;                                //最大血量  可变属性
	int32_t hp_cur;                                //当前血量  可变属性

	uint32_t strength;                             //力量
	uint32_t brain;                                //智力
	uint32_t charm;                                //魅力
	uint32_t phy_attack;                           //物理攻击
	uint32_t magic_attack;                         //法术攻击
	uint32_t phy_defence;                          //物理防御
	uint32_t magic_defence;                        //法术防御
	uint32_t parry_rate;                           //招架几率
	uint32_t counterattack_rate;                   //反击几率
	uint32_t hit_rate;                             //命中几率
	uint32_t jink_rate;                            //闪避几率
	uint32_t crit_rate;                            //暴击几率
	uint32_t speed;                                //速度

	uint32_t move_speed;                           //移动速度	
	
	PATH path;                                     //行走路径
	struct timeval last_refresh_time;              //行走路径最近更新时间
	IDX scene;                                     //所在场景
	IDX instance;                                 //所在副本
	uint16_t instance_pos;                         //在副本中的位置 (x << 8 | y)
	uint8_t instance_dice;                        //骰子的数字
	uint32_t active_skill[MAX_ACTIVE_SKILL];       //设置要释放的技能
	uint32_t skill[MAX_SKILL_PER_ROLE];            //所学的技能
	IDX bag;                                       //背包
	IDX store;                                     //仓库
} ROLE, *handle_role;

void reset_role_attribute(ROLE *role);

void init_role_data(ROLE *role);
int resume_role_data(ROLE *role);

inline int give_role_thing(ROLE *role, THING_OBJ *thing);
inline int delete_role_thing(ROLE *role, THING_OBJ *thing);
inline int delete_role_thing_byid(ROLE *role, uint32_t id, uint16_t num);

int add_one_role(ROLE *role);
int del_one_role(ROLE *role);
ROLE *find_role_by_roleid(uint64_t role_id);
ROLE *find_role_by_fd(uint16_t fd);

void set_role_scene(ROLE *role, struct scene_struct *scene);
void set_role_bag(ROLE *role, CONTAINER *bag);
void dump_role_summary(ROLE *role, RoleSummary *summary);

//不需要返回了，如果失败直接踢线，记日志
inline void role_login(ROLE *role);
inline int role_logout(ROLE *role);
inline int kick_role(ROLE *role, uint32_t reason);

void test_add_role();
#endif
