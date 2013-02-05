#include "buff.h"
#include "config.h"

int add_buff(ONE_ROLE *one_role, uint32_t id)
{
	BUFF_DATA data;
	CFGATTRIBUTE *cfg_attr;
	CFGSKILL *config = get_skill_config_byid(id);
	if (!config/* || config->continue_time < 1*/) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: [%s] find buff config failed[%u]", __FUNCTION__, one_role->attr.name, id);				
		return (-1);
	}
	cfg_attr = get_attribute_config_byid(config->attribute);
	if (!cfg_attr) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: [%s] find attribute config failed[%u]", __FUNCTION__, one_role->attr.name, id);				
		return (-1);
	}

	one_role->attr.phy_attack += cfg_attr->phy_attack;
	one_role->attr.phy_defence += cfg_attr->phy_defence;
	one_role->attr.magic_attack += cfg_attr->magic_attack;
	one_role->attr.magic_defence += cfg_attr->magic_defence;
	one_role->attr.parry_rate += cfg_attr->parry_rate;
	one_role->attr.counterattack_rate += cfg_attr->countattack_rate;
	one_role->attr.hit_rate += cfg_attr->hit_rate;
	one_role->attr.jink_rate += cfg_attr->jink_rate;
	one_role->attr.crit_rate += cfg_attr->crit_rate;
	one_role->attr.speed += cfg_attr->speed;	

	data.id = id;
//	data.continue_time = config->continue_time;
	
	return add_one_role_buff(one_role, &data);
}

int remove_buff(ONE_ROLE *one_role, uint32_t id)
{
	CFGATTRIBUTE *cfg_attr;
	CFGSKILL *config = get_skill_config_byid(id);
	if (!config/* || config->continue_time < 1*/) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: [%s] find buff config failed[%u]", __FUNCTION__, one_role->attr.name, id);				
		return (-1);
	}
	cfg_attr = get_attribute_config_byid(config->attribute);
	if (!cfg_attr) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: [%s] find attribute config failed[%u]", __FUNCTION__, one_role->attr.name, id);				
		return (-1);
	}

	one_role->attr.phy_attack -= cfg_attr->phy_attack;
	one_role->attr.phy_defence -= cfg_attr->phy_defence;
	one_role->attr.magic_attack -= cfg_attr->magic_attack;
	one_role->attr.magic_defence -= cfg_attr->magic_defence;
	one_role->attr.parry_rate -= cfg_attr->parry_rate;
	one_role->attr.counterattack_rate -= cfg_attr->countattack_rate;
	one_role->attr.hit_rate -= cfg_attr->hit_rate;
	one_role->attr.jink_rate -= cfg_attr->jink_rate;
	one_role->attr.crit_rate -= cfg_attr->crit_rate;
	one_role->attr.speed -= cfg_attr->speed;	

	return remove_one_role_buff(one_role, id);
}
