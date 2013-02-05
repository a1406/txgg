#include <assert.h>
#include "game_srv.h"
#include "game_net.h"
#include "client_map.h"
#include "server_proto.h"
#include "cs.pb-c.h"
#include "container.h"
#include "special_proto.h"

static int init_role_data_from_db(DB_ROLE_INFO *info, ROLE *role)
{
	CONTAINER *bag;
	SCENE *scene;
	int i, j;
	int ret = 0;
	DbBlobRoleInfo *blob_info;

	assert(info);

	role->money = info->money;
	role->gold = info->gold;	
	
	scene = get_scene_by_id(info->scene_id);
	if (!scene) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%lu] scene can not found %u", __FUNCTION__, role->role_id, info->scene_id);
		ret = (-1);
		goto done;
	}

	bag = create_container();
	if (unlikely(!bag)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%lu] create role bag failed %u", __FUNCTION__, role->role_id);
		ret = (-5);
		goto done;
	}
	set_role_bag(role, bag);

//	role->one_role[0].profession = info->profession;
//	role->one_role[0].attr.level = info->level;
	set_role_scene(role, scene);
//	memcpy(role->one_role[0].attr.name, info->name, MAX_ROLE_NAME_LEN);
	role->path.begin.pos_x = info->pos.pos_x;
	role->path.begin.pos_y = info->pos.pos_y;
	role->path.end.pos_x = info->pos.pos_x;
	role->path.end.pos_y = info->pos.pos_y;

	blob_info = db_blob_role_info__unpack(NULL, info->blob_size, info->blob_info);
	if (unlikely(!blob_info)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%lu] unpack role  failed %u", __FUNCTION__, role->role_id);
		ret = (-6);
		goto done;		
	}

	bag->max_thing_num = blob_info->bag->max_num;
	for (i = 0; i < blob_info->bag->n_things; ++i) {
		add_thing_bypos(bag, blob_info->bag->things[i]->pos,
			blob_info->bag->things[i]->id,
			blob_info->bag->things[i]->num);
	}
	role->quest_num = blob_info->quest->n_quest;
	for (i = 0; i < blob_info->quest->n_quest; ++i) {
		role->quest[i].id = blob_info->quest->quest[i]->id;
		role->quest[i].state = blob_info->quest->quest[i]->state;		
		role->quest[i].accept_time = blob_info->quest->quest[i]->accept_time;
		role->quest[i].cycle_num = blob_info->quest->quest[i]->cycle_num;
		role->quest[i].kill_npc1_num = blob_info->quest->quest[i]->kill_npc1_num;
		role->quest[i].kill_npc2_num = blob_info->quest->quest[i]->kill_npc2_num;
		role->quest[i].kill_npc3_num = blob_info->quest->quest[i]->kill_npc3_num;
		role->quest[i].collect_item1_num = blob_info->quest->quest[i]->collect_item1_num;
		role->quest[i].collect_item2_num = blob_info->quest->quest[i]->collect_item2_num;
		role->quest[i].collect_item3_num = blob_info->quest->quest[i]->collect_item3_num;		
	}

	role->main_quest_index = blob_info->quest->main_quest_index;
	role->finished_slave_quest_num = blob_info->quest->n_all_finished_slave_quest;
	for (i = 0; i < role->finished_slave_quest_num; ++i) {
		role->all_finished_slave_quest[i] = blob_info->quest->all_finished_slave_quest[i];		
	}

	for (i = 0; i < blob_info->n_shortcut; ++i) {
		role->shortcut[i].id = blob_info->shortcut[i]->id;
		role->shortcut[i].pos = blob_info->shortcut[i]->pos;		
	}
	for (; i < MAX_SHORTCUT_PER_ROLE; ++i) {
		role->shortcut[i].id = 0;
		role->shortcut[i].pos = 0;		
	}
	
	for (i = 0; i < blob_info->n_one_role; ++i) {
		role->one_role[i].attr.level =
			blob_info->one_role[i]->level;
		role->one_role[i].profession = blob_info->one_role[i]->prof;
		role->one_role[i].exp = blob_info->one_role[i]->exp;
		if (reset_exp_levelup(&role->one_role[i]) != 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: role[%lu] friend[%d] can not reset exp_levelup\n", __FUNCTION__, role->role_id, i);			
		}
		strcpy(role->one_role[i].attr.name, blob_info->one_role[i]->name);
		for (j = 0; j < MAX_EQUIP_PER_ROLE; ++j) {
			role->one_role[i].equip[j] = blob_info->one_role[i]->equip[j];
		}
		for (j = 0; j < MAX_ACTIVE_SKILL; ++j) {
			role->one_role[i].attr.active_skill[j] = blob_info->one_role[i]->active_skill[j];
		}
		for (j = 0; j < blob_info->one_role[i]->n_skill; ++j) {
			role->one_role[i].skill[j] = blob_info->one_role[i]->skill[j];
		}
		role->one_role[i].skill_num = blob_info->one_role[i]->n_skill;

			//todo for test
		if (role->one_role[i].skill_num == 0) {
			role->one_role[i].skill_num = 1;
			role->one_role[i].skill[0] = 1;			
		}
		
	}

	for (; i < MAX_FRIEND_PER_ROLE; ++i) {
		role->one_role[i].attr.name[0] = '\0';		
		role->one_role[i].profession = 0;
	}
	reset_role_attribute(role);

		//好友信息
	for (i = 0; i < blob_info->game_friend->n_friend_; ++i) {
		role->game_friend.friend[i] = blob_info->game_friend->friend_[i];
	}
	for (; i < MAX_GAME_FRIEND_PER_ROLE; ++i) {
		role->game_friend.friend[i] = 0;
	}
	for (i = 0; i < blob_info->game_friend->n_blacklist; ++i) {
		role->game_friend.blacklist[i] = blob_info->game_friend->blacklist[i];		
	}
	for (; i < MAX_GAME_FRIEND_PER_ROLE; ++i) {
		role->game_friend.blacklist[i] = 0;
	}	
	
done:	
	if (blob_info)
		db_blob_role_info__free_unpacked(blob_info, NULL);

	if (ret != 0) {
		kick_role(role, ret);
	}
	return (ret);
	
}

int send_login_resp(FETCH_ROLE_DATA_RESPONSE *fetch_role)
{
	assert(fetch_role);
	ROLE *role;	
	LoginResp msg;
	RoleSightInfo info;
	QuestProgressListResponse quest_data;
	RoleSummary summary[MAX_FRIEND_PER_ROLE];
	RoleSummary *summary_point[MAX_FRIEND_PER_ROLE];
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	size_t size;
	int i;
	QuestProgress quest[MAX_QUEST_PER_ROLE];
	QuestProgress *quest_point[MAX_QUEST_PER_ROLE];	
	uint32_t shortcut_id[MAX_SHORTCUT_PER_ROLE];
	uint32_t shortcut_pos[MAX_SHORTCUT_PER_ROLE];	
	
	login_resp__init(&msg);
	role_sight_info__init(&info);
	msg.info = &info;
	quest_progress_list_response__init(&quest_data);
	msg.quest = &quest_data;
	
	role = get_role_by_id(fetch_role->id);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: can not find role %lu", __FUNCTION__, fetch_role->id);
		return (-1);
	}
	msg.result = fetch_role->result;	
	if (msg.result != 0) {
		goto done;
	}

	init_role_data_from_db(&fetch_role->info, role);
	
//暂时没有宝宝	
	role_summary__init(&summary[0]);
	summary_point[0] = &summary[0];
	msg.role_summary = summary_point;
	msg.n_role_summary = 1;

	info.role_id = fetch_role->id & 0xffffffff;
	info.area_id = fetch_role->id >> 32;
	info.move_start_x = fetch_role->info.pos.pos_x;
	info.move_start_y = fetch_role->info.pos.pos_y;
	info.move_end_x = fetch_role->info.pos.pos_x;
	info.move_end_x = fetch_role->info.pos.pos_y;	
	info.profession = fetch_role->info.profession;
	info.name = role->one_role[0].attr.name;
	info.level = role->one_role[0].attr.level;
	info.move_speed = role->move_speed;

	msg.n_shutcut_pos = MAX_SHORTCUT_PER_ROLE;
	msg.n_shutcut_id = MAX_SHORTCUT_PER_ROLE;
	msg.shutcut_pos = shortcut_pos;
	msg.shutcut_id = shortcut_id;	
	for (i = 0; i < MAX_SHORTCUT_PER_ROLE; ++i) {
		shortcut_pos[i] = role->shortcut[i].pos;
		shortcut_id[i] = role->shortcut[i].id;		
	}

	quest_data.quest = quest_point;
	for (i = 0; i < role->quest_num; ++i) {
		assert(role->quest[i].id != (-1));

		quest_point[i] = &quest[i];
		quest_progress__init(quest_point[i]);
		quest[i].id = role->quest[i].id;
		quest[i].state = role->quest[i].state;
		quest[i].accept_time = role->quest[i].accept_time;
		quest[i].cycle_num = role->quest[i].cycle_num;
		quest[i].kill_npc1_num = role->quest[i].kill_npc1_num;
		quest[i].kill_npc2_num = role->quest[i].kill_npc2_num;
		quest[i].kill_npc3_num = role->quest[i].kill_npc3_num;
		quest[i].collect_item1_num = role->quest[i].collect_item1_num;
		quest[i].collect_item2_num = role->quest[i].collect_item2_num;
		quest[i].collect_item3_num = role->quest[i].collect_item3_num;
	}
	quest_data.n_quest = i;
	quest_data.main_quest_index = role->main_quest_index;
	quest_data.n_all_finished_slave_quest = role->finished_slave_quest_num;
	quest_data.all_finished_slave_quest = role->all_finished_slave_quest;
	
	msg.scene_id = fetch_role->info.scene_id;
	msg.money = role->money;
	msg.gold = role->gold;	

	for (i = 0; i < MAX_FRIEND_PER_ROLE; ++i) {
		if (!is_role_friend_exist(role, i))
			break;

		summary[i].strength = role->one_role[i].strength;
		summary[i].brain = role->one_role[i].brain;
		summary[i].charm = role->one_role[i].charm;
		summary[i].agile = role->one_role[i].agile;
			
		summary[i].profession = role->one_role[i].profession;
		summary[i].level = role->one_role[i].attr.level;
		summary[i].name = role->one_role[i].attr.name;
		summary[i].n_skill = role->one_role[i].skill_num;
		summary[i].skill = role->one_role[i].skill;
		summary[i].active_skill1 = role->one_role[i].attr.active_skill[0];
		summary[i].active_skill2 = role->one_role[i].attr.active_skill[1];
		summary[i].active_skill3 = role->one_role[i].attr.active_skill[2];

		summary[i].hp_max = role->one_role[i].attr.hp_cur;
		summary[i].hp_cur = role->one_role[i].attr.hp_cur;
		summary[i].phy_attack = role->one_role[i].attr.phy_attack;
		summary[i].phy_defence = role->one_role[i].attr.phy_defence;
		summary[i].magic_attack = role->one_role[i].attr.magic_attack;
		summary[i].magic_defence = role->one_role[i].attr.magic_defence;
		summary[i].parry_rate = role->one_role[i].attr.parry_rate;
		
		summary[i].counterattack_rate = role->one_role[i].attr.counterattack_rate;
		summary[i].hit_rate = role->one_role[i].attr.hit_rate;
		summary[i].jink_rate = role->one_role[i].attr.jink_rate;
		summary[i].crit_rate = role->one_role[i].attr.crit_rate;
		summary[i].speed = role->one_role[i].attr.speed;
		summary[i].exp = role->one_role[i].exp;

		summary[i].n_equip = MAX_EQUIP_PER_ROLE;
		summary[i].equip = role->one_role[i].equip;
/*		
	summary[0].strength
	summary[0].brain
	summary[0].charm
*/
	}
done:	
	size = login_resp__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));

	size += sizeof(PROTO_HEAD_CONN);
	head->msg_id = htons(CS__MESSAGE__ID__LOGIN_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send login resp to %lu failed", __FUNCTION__, fetch_role->id);		
	}

	role_login(role);
	return (0);
}

int send_createrole_resp(CREATE_ROLE_RESPONSE *create_role)
{
	assert(create_role);
	ROLE *role;
	CreateroleResp msg;
	RoleSightInfo info;		
	RoleSummary summary;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	size_t size;
	QuestProgressListResponse quest_data;	
	QuestProgress quest[MAX_QUEST_PER_ROLE];
	QuestProgress *quest_point[MAX_QUEST_PER_ROLE];		
	int i;
	role = get_role_by_id(create_role->id);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: can not find role %lu", __FUNCTION__, create_role->id);
		return (-1);
	}

	createrole_resp__init(&msg);
	role_sight_info__init(&info);
	msg.info = &info;
	quest_progress_list_response__init(&quest_data);
	msg.quest = &quest_data;
	
	role_summary__init(&summary);
	msg.role_summary = &summary;
	
	msg.result = create_role->result;	
	if (msg.result != 0)
		goto done;

	init_role_data_from_db(&create_role->info, role);	
	
	
	msg.scene_id = create_role->info.scene_id;	
	info.role_id = create_role->id & 0xffffffff;
	info.area_id = create_role->id >> 32;
	info.move_start_x = create_role->info.pos.pos_x;
	info.move_start_y = create_role->info.pos.pos_y;
	info.move_end_x = create_role->info.pos.pos_x;
	info.move_end_y = create_role->info.pos.pos_y;
	info.profession = create_role->info.profession;
	info.name = role->one_role[0].attr.name;
	info.level = role->one_role[0].attr.level;
	info.move_speed = role->move_speed;

	quest_data.quest = quest_point;
	for (i = 0; i < role->quest_num; ++i) {
		assert(role->quest[i].id != (-1));

		quest_point[i] = &quest[i];
		quest_progress__init(quest_point[i]);
		quest[i].id = role->quest[i].id;
		quest[i].state = role->quest[i].state;
		quest[i].accept_time = role->quest[i].accept_time;
		quest[i].cycle_num = role->quest[i].cycle_num;
		quest[i].kill_npc1_num = role->quest[i].kill_npc1_num;
		quest[i].kill_npc2_num = role->quest[i].kill_npc2_num;
		quest[i].kill_npc3_num = role->quest[i].kill_npc3_num;
		quest[i].collect_item1_num = role->quest[i].collect_item1_num;
		quest[i].collect_item2_num = role->quest[i].collect_item2_num;
		quest[i].collect_item3_num = role->quest[i].collect_item3_num;
	}
	quest_data.n_quest = i;
	quest_data.main_quest_index = role->main_quest_index;
	quest_data.n_all_finished_slave_quest = role->finished_slave_quest_num;
	quest_data.all_finished_slave_quest = role->all_finished_slave_quest;

	summary.strength = role->one_role[0].strength;
	summary.brain = role->one_role[0].brain;
	summary.charm = role->one_role[0].charm;
	summary.agile = role->one_role[0].agile;
	
	summary.profession = role->one_role[0].profession;
	summary.level = role->one_role[0].attr.level;
	summary.name = role->one_role[0].attr.name;
	summary.n_skill = role->one_role[0].skill_num;
	summary.skill = role->one_role[0].skill;
	summary.active_skill1 = role->one_role[0].attr.active_skill[0];
	summary.active_skill2 = role->one_role[0].attr.active_skill[1];
	summary.active_skill3 = role->one_role[0].attr.active_skill[2];
	summary.hp_max = role->one_role[0].attr.hp_cur;
	summary.hp_cur = role->one_role[0].attr.hp_cur;
	summary.phy_attack = role->one_role[0].attr.phy_attack;
	summary.phy_defence = role->one_role[0].attr.phy_defence;
	summary.magic_attack = role->one_role[0].attr.magic_attack;
	summary.magic_defence = role->one_role[0].attr.magic_defence;
	summary.parry_rate = role->one_role[0].attr.parry_rate;
 	summary.counterattack_rate = role->one_role[0].attr.counterattack_rate;
	summary.hit_rate = role->one_role[0].attr.hit_rate;
	summary.jink_rate = role->one_role[0].attr.jink_rate;
	summary.crit_rate = role->one_role[0].attr.crit_rate;
	summary.speed = role->one_role[0].attr.speed;
	summary.exp = role->one_role[0].exp;
	
done:	
	size = createrole_resp__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	head->msg_id = htons(CS__MESSAGE__ID__CREATEROLE_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send login resp to %lu failed", __FUNCTION__, create_role->id);		
	}
	
	role_login(role);	
	return (0);
}


int handle_roledb_msg(CLIENT_MAP *roledb_server)
{
	PROTO_HEAD *head;

	CREATE_ROLE_RESPONSE *create_role;
	FETCH_ROLE_DATA_RESPONSE *fetch_role;
	STORE_ROLE_DATA_RESPONSE *store_role;		
	
	assert(roledb_server);

	head = (PROTO_HEAD *)(&roledb_server->buf[0]);

	switch (htons(head->msg_id))
	{
		case SERVER_PROTO_FETCH_ROLE_RESPONSE:
			fetch_role = (FETCH_ROLE_DATA_RESPONSE *)(&head->data[0]);
			send_login_resp(fetch_role);
			break;
		case SERVER_PROTO_CREATE_ROLE_RESPONSE:
			create_role = (CREATE_ROLE_RESPONSE *)(&head->data[0]);
			send_createrole_resp(create_role);
			break;
		case SERVER_PROTO_STORE_ROLE_RESPONSE:
			store_role = (STORE_ROLE_DATA_RESPONSE *)(&head->data[0]);
			log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s: role %lu logout return %d\n",
				__FUNCTION__, store_role->id, store_role->result);			
			break;
		default:
			log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
				"%s: get useless msg id[]", __FUNCTION__, htons(head->msg_id));
			break;			
	}

//	fetch_role = fetch_role;
//	create_role = create_role;	
	return (0);
}

int test_send_fetch_role(uint16_t fd)
{
	char sendbuf[sizeof(PROTO_HEAD) + sizeof(FETCH_ROLE_DATA_REQUEST)];
	PROTO_HEAD *head = (PROTO_HEAD *)(&sendbuf[0]);
	FETCH_ROLE_DATA_REQUEST *req = (FETCH_ROLE_DATA_REQUEST *)(&sendbuf[sizeof(PROTO_HEAD)]);	

	head->len = htons(sizeof(sendbuf));
	head->msg_id = htons(SERVER_PROTO_FETCH_ROLE_REQUEST);
	req->id = 8686l;
	send_one_msg(fd, head, 0);
	return (0);
}
