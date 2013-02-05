#include <stdint.h>
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

typedef struct buff_data
{
	uint32_t id;
	int32_t continue_time;
} BUFF_DATA;

//战斗相关的属性
typedef struct attribute
{
	char name[MAX_ROLE_NAME_LEN];                  //角色名      DB
	int32_t hp_cur;                                //当前血量  可变属性
	int32_t hp_max;                                //最大血量  可变属性
	uint16_t level;                                //等级  可变属性   DB
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
	uint8_t buff_num;	
	BUFF_DATA buff[MAX_BUFF_PER_ROLE];	           //buff
	uint32_t active_skill[MAX_ACTIVE_SKILL];       //设置要释放的技能  DB
} ATTRIBUTE;

//主角，宝宝，怪物通用
typedef struct one_role
{
	uint32_t profession;                            //职业,对宝宝来说，就是NPCID  不可变属性   DB
	uint32_t strength;                             //力量
	uint32_t brain;                                //智力
	uint32_t charm;                                //魅力
	uint32_t agile;                                //敏捷
	uint32_t exp;                                  //经验              DB
	uint32_t exp_levelup;                          //升级所需经验
	ATTRIBUTE attr;
	uint8_t skill_num;                             //                 DB
	uint32_t skill[MAX_SKILL_PER_ROLE];            //所学的技能        DB
	uint32_t equip[MAX_EQUIP_PER_ROLE];            //equip           DB
} ONE_ROLE;

inline int reset_exp_levelup(ONE_ROLE *one_role);
inline void reset_one_role_attribute(ONE_ROLE *one_role);
inline int fill_one_role_attr_changed(ONE_ROLE *one_role, uint8_t pos, AttrChanged *data);

int add_one_role_buff(ONE_ROLE *one_role, BUFF_DATA *data);
int remove_one_role_buff(ONE_ROLE *one_role, uint32_t id);
BUFF_DATA *get_one_role_buff(ONE_ROLE *one_role, uint32_t id);

int add_one_role_skill(ONE_ROLE *one_role, uint32_t id);
int search_one_role_skill(ONE_ROLE *one_role, uint32_t id);
int delete_one_role_skill(ONE_ROLE *one_role, uint32_t id);
