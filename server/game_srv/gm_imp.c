#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "gm.h"
#include "game_srv.h"
#include "game_net.h"

static int gm_setrole_handle(ROLE *role, int param_num, char param[MAX_GAMEMASTER_PARAMETER_NUMBER][MAX_GAMEMASTER_PER_PARAMETER_LENGTH])
{
	int value;

		//check param_num
	if (param_num != 2)
		return (-1);
	
	value = atoi(param[1]);


	if (strcmp(param[0], "hp") == 0) {
		role->one_role[0].attr.hp_max = value;
	} else if (strcmp(param[0], "pa") == 0) {
		role->one_role[0].attr.phy_attack = value;
	} else if (strcmp(param[0], "pd") == 0) {
		role->one_role[0].attr.phy_defence = value;		
	} else if (strcmp(param[0], "parry") == 0) {
		role->one_role[0].attr.parry_rate = value;				
	} else if (strcmp(param[0], "counterattack") == 0) {
		role->one_role[0].attr.counterattack_rate = value;						
	} else if (strcmp(param[0], "hit") == 0) {
		role->one_role[0].attr.hit_rate = value;								
	} else if (strcmp(param[0], "jink") == 0) {
		role->one_role[0].attr.jink_rate = value;								
	} else if (strcmp(param[0], "crit") == 0) {
		role->one_role[0].attr.crit_rate = value;										
	} else if (strcmp(param[0], "speed") == 0) {
		role->one_role[0].attr.speed = value;												
	} else if (strcmp(param[0], "move_speed") == 0) {
		role->move_speed = value;
	} else if (strcmp(param[0], "prof") == 0) {
		role->one_role[0].profession = value;
	} else if (strcmp(param[0], "level") == 0) {
		role->one_role[0].attr.level = value;				
	} else {
		return (-2);
	}
	return (0);
}

static int gm_quest_handle(ROLE *role, int param_num, char param[MAX_GAMEMASTER_PARAMETER_NUMBER][MAX_GAMEMASTER_PER_PARAMETER_LENGTH])
{
	int value;
	DB_QUEST_INFO info; 	
	if (param_num != 2)
		return (-1);
	value = atoi(param[1]);
	if (strcmp(param[0], "accept") == 0) {
		info.id = value;
		add_role_quest(role, &info);
	} else if (strcmp(param[0], "cancel") == 0) {
		remove_role_quest(role, value);
	} else if (strcmp(param[0], "complete") == 0) {
		remove_role_quest(role, value);		
	} else {
		return (-2);
	}
	return (0);
}

static int gm_skill_handle(ROLE *role, int param_num, char param[MAX_GAMEMASTER_PARAMETER_NUMBER][MAX_GAMEMASTER_PER_PARAMETER_LENGTH])
{
	int len, ret;
	int i;
	int info_size;
	char info[512];
	int value;
	size_t size;	
	PROTO_HEAD_CONN *head;
	GmNotify notify;
	SkillLearnResponse response;	
	head = (PROTO_HEAD_CONN *)toclient_send_buf;
	
	if (param_num != 2)
		return (-1);
	value = atoi(param[1]);
	
	if (strcmp(param[0], "add") == 0) {
		add_one_role_skill(&role->one_role[0], value);
		skill_learn_response__init(&response);
		response.id = value;
		response.n_skills = role->one_role[0].skill_num;
		response.skills = &role->one_role[0].skill[0];
		size = skill_learn_response__pack(&response, (uint8_t *)head->data);
		len = size + sizeof(PROTO_HEAD_CONN);
		head->len = htons(len);
		head->msg_id = htons(CS__MESSAGE__ID__SKILL_LEARN_RESPONSE);
		head->fd = ROLE_FD(role);
		ret = send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0);
		if (ret != len) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: send_one_msg fail len[%d] ret[%d] errno[%d]\n", __FUNCTION__, len, ret, errno);			
		}
	} else if (strcmp(param[0], "del") == 0) {
		delete_one_role_skill(&role->one_role[0], value);
	} else if (strcmp(param[0], "show") == 0) {
		gm_notify__init(&notify);
		info_size = 0;
		for (i = 0; i < role->one_role[0].skill_num; ++i) {
			info_size += sprintf(&info[info_size], " %u", role->one_role[0].skill[i]);
		}
		notify.msg = info;
		size = gm_notify__pack(&notify, (uint8_t *)head->data);
		len = size + sizeof(PROTO_HEAD_CONN);
		head->len = htons(len);
		head->msg_id = htons(CS__MESSAGE__ID__GM_NOTIFY);
		head->fd = ROLE_FD(role);
		ret = send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0);
		if (ret != len) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: send_one_msg fail len[%d] ret[%d] errno[%d]\n", __FUNCTION__, len, ret, errno);			
		}
	} else {
		return (-2);
	}
	return (0);
}

static int gm_item_handle(ROLE *role, int param_num, char param[MAX_GAMEMASTER_PARAMETER_NUMBER][MAX_GAMEMASTER_PER_PARAMETER_LENGTH])
{
	int value;
	struct thing_list list;
	if (param_num != 1)
		return (-1);
	value = atoi(param[0]);
/*
	thing = create_thing_obj();
	if (!thing)
		return (-2);

	thing->id = value;
	thing->num = 1;
	give_role_thing(role, thing);
*/
	list.num = 1;
	list.things[0].id = value;
	list.things[0].num = 1;	
	give_role_thing_list(role, &list);
	return (0);
}

static int gm_dice_handle(ROLE *role, int param_num, char param[MAX_GAMEMASTER_PARAMETER_NUMBER][MAX_GAMEMASTER_PER_PARAMETER_LENGTH])
{
	int value;
	if (param_num != 1)
		return (-1);
	value = atoi(param[0]);

	role->instance_data.special_dice = value;
	return (0);
}

static int gm_drop_handle(ROLE *role, int param_num, char param[MAX_GAMEMASTER_PARAMETER_NUMBER][MAX_GAMEMASTER_PER_PARAMETER_LENGTH])
{
	int value;
	if (param_num != 1)
		return (-1);
	value = atoi(param[0]);

	give_role_drop(role, value, NULL);
	return (0);
}

static int gm_info_handle(ROLE *role, int param_num, char param[MAX_GAMEMASTER_PARAMETER_NUMBER][MAX_GAMEMASTER_PER_PARAMETER_LENGTH])
{
	int len;
	int ret;
	char info[512];
	size_t size;	
	PROTO_HEAD_CONN *head;
	GmNotify notify;
	head = (PROTO_HEAD_CONN *)toclient_send_buf;
	gm_notify__init(&notify);

	sprintf(info, "name[%s] hp[%d] exp[%d] prof[%d] level[%d] pa[%d] pd[%d] parry[%d] counterattack[%d] hit[%d] jink[%d] crit[%d] speed[%d] move_speed[%d]",
		role->one_role[0].attr.name, role->one_role[0].attr.hp_max, role->one_role[0].exp, role->one_role[0].profession,
		role->one_role[0].attr.level, role->one_role[0].attr.phy_attack, role->one_role[0].attr.phy_defence,
		role->one_role[0].attr.parry_rate, role->one_role[0].attr.counterattack_rate, role->one_role[0].attr.hit_rate,
		role->one_role[0].attr.jink_rate, role->one_role[0].attr.crit_rate, role->one_role[0].attr.speed, role->move_speed);
	notify.msg = info;
	
	size = gm_notify__pack(&notify, (uint8_t *)head->data);
	len = size + sizeof(PROTO_HEAD_CONN);
	head->len = htons(len);
	head->msg_id = htons(CS__MESSAGE__ID__GM_NOTIFY);
	head->fd = ROLE_FD(role);
	ret = send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0);
	if (ret != len) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send_one_msg fail len[%d] ret[%d] errno[%d]\n", __FUNCTION__, len, ret, errno);			
	}
	return (0);
}

int init_gm_system()
{
	register_gm_handle(gm_setrole_handle, "setrole");
	register_gm_handle(gm_quest_handle, "quest");
	register_gm_handle(gm_info_handle, "info");
	register_gm_handle(gm_skill_handle, "skill");
	register_gm_handle(gm_item_handle, "item");
	register_gm_handle(gm_dice_handle, "dice");
	register_gm_handle(gm_drop_handle, "drop");
	return (0);
}
