#include "one_role.h"
#include "config.h"

inline static int comp_role_buff(const void *a, const void *b)
{
	if (((BUFF_DATA *)a)->id == ((BUFF_DATA *)b)->id)
		return (0);
	if (((BUFF_DATA *)a)->id > ((BUFF_DATA *)b)->id)	
		return (1);
	return (-1);
}

int search_one_role_skill(ONE_ROLE *one_role, uint32_t id)
{
	int find, index;	
	index = array_bsearch(&id, one_role->skill, one_role->skill_num, sizeof(id), &find, comp_uint32);
	if (find == 0)
		return (-1);
	return index;
}

int add_one_role_skill(ONE_ROLE *one_role, uint32_t id)
{
	if (one_role->skill_num >= MAX_SKILL_PER_ROLE)
		return (-1);
	if (array_insert(&id, one_role->skill, (int *)&one_role->skill_num, sizeof(id), 1, comp_uint32) < 0)
		return (-10);
	return (0);
}
int delete_one_role_skill(ONE_ROLE *one_role, uint32_t id)
{
	if (array_delete(&id, one_role->skill, (int *)&one_role->skill_num, sizeof(id), comp_uint32) < 0)
		return (-10);
	return (0);
}

int add_one_role_buff(ONE_ROLE *one_role, BUFF_DATA *data)
{
	int index;
	if (one_role->attr.buff_num >= MAX_BUFF_PER_ROLE)
		return (-1);

	index = array_insert(data, one_role->attr.buff, (int *)&one_role->attr.buff_num, sizeof(*data), 1, comp_role_buff);
	return index;
}
int remove_one_role_buff(ONE_ROLE *one_role, uint32_t id)
{
	BUFF_DATA data;
	data.id = id;	
	return array_delete(&data, one_role->attr.buff, (int *)&one_role->attr.buff_num, sizeof(data), comp_role_buff);
}
BUFF_DATA *get_one_role_buff(ONE_ROLE *one_role, uint32_t id)
{
	int find, index;
	BUFF_DATA data;
	data.id = id;

	index = array_bsearch(&data, one_role->attr.buff, one_role->attr.buff_num, sizeof(data), &find, comp_role_buff);
	if (find == 0)
		return NULL;
	assert(index < one_role->attr.buff_num);
	return &one_role->attr.buff[index];
}

inline int reset_exp_levelup(ONE_ROLE *one_role)
{
	assert(one_role);
	CFGPLAYER *config = get_player_config_byid(one_role->profession, one_role->attr.level);
	if (!config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: get player config %u %u fail\n", __FUNCTION__, one_role->profession, one_role->attr.level);
		one_role->exp_levelup = 0xffffffff;		
		return (-1);
	}
	one_role->exp_levelup = config->exp;
	return (0);
}

inline void reset_one_role_attribute(ONE_ROLE *one_role)
{
	CFGPLAYER *role_cfg;
	CFGATTRIBUTE *attr_cfg;	
	
	if (!one_role || one_role->attr.name[0] == '\0')
		return;
	role_cfg = get_player_config_byid(one_role->profession, one_role->attr.level);
	if (!role_cfg) {
		return;
	}
	attr_cfg = get_attribute_config_byid(role_cfg->attribute);
	if (!attr_cfg) {
		return;
	}
		
	one_role->attr.hp_max = attr_cfg->hp;
	one_role->attr.hp_cur = one_role->attr.hp_max;
	one_role->strength = attr_cfg->strength;
	one_role->brain = attr_cfg->brain;
	one_role->charm = attr_cfg->charm;
	one_role->agile = attr_cfg->agile;
	
	one_role->attr.phy_attack = attr_cfg->phy_attack;
	one_role->attr.phy_defence = attr_cfg->phy_defence;
	one_role->attr.magic_attack = attr_cfg->magic_attack;
	one_role->attr.magic_defence = attr_cfg->magic_defence;
	one_role->attr.parry_rate = attr_cfg->parry_rate;
	one_role->attr.counterattack_rate = attr_cfg->countattack_rate;
	one_role->attr.hit_rate = attr_cfg->hit_rate;
	one_role->attr.jink_rate = attr_cfg->jink_rate;
	one_role->attr.crit_rate = attr_cfg->crit_rate;
	one_role->attr.speed = attr_cfg->speed;
}

inline int fill_one_role_attr_changed(ONE_ROLE *one_role, uint8_t pos, AttrChanged *data)
{
	assert(one_role);
	assert(data);
	int i = 0;

	attr_changed__init(&data[i]);	
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__HP_MAX;
	data[i].new_value = one_role->attr.hp_max;
	++i;

	attr_changed__init(&data[i]);		
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__HP_CUR;
	data[i].new_value = one_role->attr.hp_cur;
	++i;
	
	attr_changed__init(&data[i]);			
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__PHY_ATTACK;
	data[i].new_value = one_role->attr.phy_attack;
	++i;

	attr_changed__init(&data[i]);			
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__PHY_DEFENCE;
	data[i].new_value = one_role->attr.phy_defence;
	++i;
	
	attr_changed__init(&data[i]);			
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__MAGIC_ATTACK;
	data[i].new_value = one_role->attr.magic_attack;
	++i;

	attr_changed__init(&data[i]);			
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__MAGIC_DEFENCE;
	data[i].new_value = one_role->attr.magic_defence;
	++i;
	
	attr_changed__init(&data[i]);			
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__PARRY_RATE;
	data[i].new_value = one_role->attr.parry_rate;
	++i;
	
	attr_changed__init(&data[i]);			
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__COUNTERATTACK_RATE;
	data[i].new_value = one_role->attr.counterattack_rate;
	++i;	

	attr_changed__init(&data[i]);			
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__HIT_RATE;
	data[i].new_value = one_role->attr.hit_rate;
	++i;
	
	attr_changed__init(&data[i]);			
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__JINK_RATE;
	data[i].new_value = one_role->attr.jink_rate;
	++i;
	
	attr_changed__init(&data[i]);			
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__CRIT_RATE;
	data[i].new_value = one_role->attr.crit_rate;		
	++i;
	
	attr_changed__init(&data[i]);			
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__SPEED;
	data[i].new_value = one_role->attr.speed;
	++i;
/*
	attr_changed__init(&data[12]);				
	data[12].friend_pos = pos;
	data[12].id = ATTR__ID__EXP;
	data[12].new_value = one_role->exp;
*/
	attr_changed__init(&data[i]);				
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__LEVEL;
	data[i].new_value = one_role->attr.level;	
	++i;

	attr_changed__init(&data[i]);				
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__STRENGTH;
	data[i].new_value = one_role->strength;	
	++i;
	
	attr_changed__init(&data[i]);				
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__BRAIN;
	data[i].new_value = one_role->brain;	
	++i;
	
	attr_changed__init(&data[i]);				
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__CHARM;
	data[i].new_value = one_role->charm;	
	++i;
	
	attr_changed__init(&data[i]);				
	data[i].friend_pos = pos;
	data[i].id = ATTR__ID__AGILE;
	data[i].new_value = one_role->agile;	
	++i;
	
	return i;
}
