#include <assert.h>
#include "game_srv.h"
#include "game_net.h"
#include "quest.h"
#include "role.h"
#include "cs.pb-c.h"

int quest_status_check(	ROLE *p_role, DB_QUEST_INFO *role_quest_info, CFGMAINQUEST *quest_config)
{
    int loop_num = 0, i;
    THING_OBJ * p_things;
    uint32_t item_id;

	assert(p_role);
	assert(role_quest_info);
	assert(quest_config);	

	if (p_role->main_quest_index != quest_config->id - 1) {
		return (-1);
	}
	
    if(quest_config->kill_npc) {
		role_quest_info->state = QUEST__STATE__IN_PROGRESS;
		role_quest_info->kill_npc1_num = 0;
		return 0;
	}

	item_id = quest_config->collect_item;
    
	if(!item_id) {
        role_quest_info->state = QUEST__STATE__FINISHED;
        return 0;
    }

	role_quest_info->collect_item1_num = 0;	
    loop_num  = ((CONTAINER *)p_role->bag.p)->max_thing_num;
    
    assert( loop_num < MAX_THING_OBJ_PER_CONTAINER );
    for( i = 0; i < loop_num; i++ ) {
        p_things = ( THING_OBJ * )((CONTAINER *)p_role->bag.p)->things[i].p;    
        if (p_things && p_things->id  == item_id && p_things->num > 0) {
			role_quest_info->collect_item1_num = 1;
			role_quest_info->state = QUEST__STATE__FINISHED;
			break;
        }
    }
    return 0;
}

int handle_quest_list_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	ROLE *role;	
	int i;
	size_t size;
	QuestProgressListResponse resp;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	
	role = get_role_by_session(session);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-1);
	}

	QuestProgress quest[role->quest_num];
	QuestProgress *quest_point[role->quest_num];	
	
	quest_progress_list_response__init(&resp);

	resp.quest = quest_point;
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
	resp.n_quest = i;
	resp.main_quest_index = role->main_quest_index;
	resp.n_all_finished_slave_quest = role->finished_slave_quest_num;
//	uint32_t slave_quest[role->finished_slave_quest_num];
//	for (i = 0; i < role->finished_slave_quest_num; ++i) {
//		slave_quest[i] = role->all_finished_slave_quest[i];
//	}
	resp.all_finished_slave_quest = role->all_finished_slave_quest;
	
	size = quest_progress_list_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	head->len = htons(size);
	head->fd = ROLE_FD(role);
	head->msg_id = htons(CS__MESSAGE__ID__QUEST_LIST_RESPONSE);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (0);
}

static inline int give_role_quest_reward(ROLE *role, CFGMAINQUEST *config)
{
	struct thing_list list;

	list.num = 0;	
	if (config->award_item1) {
		list.things[list.num].id = config->award_item1;
		list.things[list.num].num = config->award_item1_num;
		++list.num;
	}
	if (config->award_item2) {
		list.things[list.num].id = config->award_item2;
		list.things[list.num].num = config->award_item2_num;
		++list.num;
	}	
	
	if (list.num &&
		give_role_thing_list(role, &list) != 0) {
		return (-1);
	}

	if (config->award_exp)
		give_role_exp(role, config->award_exp);
	if (config->award_money)
		modify_role_money(role, config->award_money);
	return (0);
}

int handle_quest_operate_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	ROLE *role;
	DB_QUEST_INFO quest;
	DB_QUEST_INFO *role_quest;	
	QuestOperateRequest *req = NULL;
	QuestOperateResponse resp;
	size_t size;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;	
    CFGMAINQUEST *quest_config;

	role = get_role_by_session(session);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-10);
	}

	quest_operate_response__init(&resp);	
	
	req = quest_operate_request__unpack(NULL, len, data);
	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack failed by role[%u][%lu] failed", __FUNCTION__, session, role->role_id);
		ret = -15;
		goto done;
	}

	quest.id = req->id;
	quest_config = get_quest_main_config_byid(quest.id);
	if (!quest_config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%u][%lu] get quest %d config failed", __FUNCTION__, session, role->role_id, quest.id);
		ret = -20;
		goto done;		
	}
	
	switch (req->operator_)
	{
		case QUEST__OPERATOR__ACCEPT:
            ret = quest_status_check (role, &quest , quest_config);
            if( ret < 0 ) {
				log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
					"%s: role[%u][%lu] try to accept quest %d failed[%d]", __FUNCTION__, session, role->role_id, quest.id, ret);
                break;
			}

			add_role_quest(role, &quest);
			break;
		case QUEST__OPERATOR__CANCEL:
			remove_role_quest(role, req->id);			
			break;
		case QUEST__OPERATOR__COMPLETE:
				//1：检查是否是完成状态
                //2：更新主线任务id
			role_quest = get_role_quest(role, req->id);
			if (!role_quest || role_quest->state != QUEST__STATE__FINISHED) {
				log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
					"%s: role[%u][%lu] try to complete quest %d failed", __FUNCTION__, session, role->role_id, quest.id);
				ret = -30;
				goto done;						
			}

			if (give_role_quest_reward(role, quest_config) != 0) {
				log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
					"%s: role[%u][%lu] give quest %d reward failed", __FUNCTION__, session, role->role_id, quest.id);
				ret = -40;
				goto done;										
			}

			role->main_quest_index = req->id;
			
			remove_role_quest(role, req->id);

			trigger_script_event(role, TYPE_QUEST, req->id, QUEST_COMPLETE, 0);
			break;
		default:
			break;
	}

done:
	resp.result = ret;
	if (req) {
		resp.id = req->id;
		resp.operator_ = req->operator_;
		quest_operate_request__free_unpacked(req, NULL);
	}
	
	size = quest_operate_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	head->len = htons(size);
	head->fd = ROLE_FD(role);
	head->msg_id = htons(CS__MESSAGE__ID__QUEST_OPERATE_RESPONSE);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);		
	}
	
	return (ret);
}

int send_update_quest_notify(ROLE *role, QuestProgress *quest)
{
	QuestUpdateNotify msg;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;

	assert(role);
	assert(quest);

	quest_update_notify__init(&msg);
	msg.progress = quest;
	
	size = quest_update_notify__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__QUEST_UPDATE_NOTIFY);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);		
	}
	
	return (0);
}

int update_main_quest_state(ROLE *role, CFGMAINQUEST *config, DB_QUEST_INFO *quest)
{
	int old_state;
	QuestProgress prog;
	assert(role);
	assert(config);
	assert(quest);	

	old_state = quest->state;
	if (config->collect_item && quest->collect_item1_num == 0) {
		quest->state = QUEST__STATE__IN_PROGRESS;
		goto done;
	}
	if (config->kill_npc && quest->kill_npc1_num == 0) {
		quest->state = QUEST__STATE__IN_PROGRESS;
		goto done;
	}
	quest->state = QUEST__STATE__FINISHED;

done:	
	if (quest->state == old_state)
		return (0);

	quest_progress__init(&prog);
	prog.id = quest->id;
	prog.state = quest->state;
	send_update_quest_notify(role, &prog);
	
	return (0);
}



void on_kill_monster(ROLE *role, uint32_t param)
{
	CFGMAINQUEST *config;
	DB_QUEST_INFO *quest;

	CFGMAPFUNCTION *func = get_instance_function_byid(param);
	if (!func)
		return;
	
	config = get_quest_main_config_byid(role->main_quest_index + 1);
	if (!config) {
		return;
	}
	quest = get_role_quest(role, role->main_quest_index + 1);
	if (!quest || quest->state == QUEST__STATE__FINISHED)
		return;


	if (config->kill_npc == func->monster1) {
		quest->kill_npc1_num++;
		update_main_quest_state(role, config, quest);
	}
	return;
}

void on_bag_changed(ROLE *role)
{
	CONTAINER *bag;
	CFGMAINQUEST *config;
	DB_QUEST_INFO *quest;	
	config = get_quest_main_config_byid(role->main_quest_index + 1);
	if (!config) {
		return;
	}
	quest = get_role_quest(role, role->main_quest_index + 1);
	if (!quest)
		return;

	bag = (CONTAINER *)role->bag.p;
	quest->collect_item1_num = count_thing_num(bag, config->collect_item);	
	update_main_quest_state(role, config, quest);
	return;
}
