#ifndef _GAME_SRV_QUEST_H__
#define _GAME_SRV_QUEST_H__

#include <stdint.h>
#include "role.h"
#include "cs.pb-c.h"
#include "config.h"


int handle_quest_list_request(uint32_t session, const uint8_t *data, uint16_t len);
int handle_quest_operate_request(uint32_t session, const uint8_t *data, uint16_t len);

int send_update_quest_notify(ROLE *role, QuestProgress *quest);
int update_main_quest_state(ROLE *role, CFGMAINQUEST *config, DB_QUEST_INFO *quest);
int pre_quest_check(uint32_t id, CFGMAINQUEST *quest_config);
int quest_status_check(ROLE * p_role, DB_QUEST_INFO *role_quest_info, CFGMAINQUEST *quest_config);


void on_kill_monster(ROLE *role, uint32_t id);
void on_bag_changed(ROLE *role);
#endif
