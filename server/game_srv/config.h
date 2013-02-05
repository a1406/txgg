#ifndef _GAMESRV_CONFIG_H__
#define _GAMESRV_CONFIG_H__

#include <stdint.h>
#include "config_scene.pb-c.h"
#include "config_skill.pb-c.h"
#include "config_drop.pb-c.h"
#include "config_player.pb-c.h"
#include "config_quest_main.pb-c.h"
#include "config_attribute.pb-c.h"
#include "config_NPC.pb-c.h"
#include "config_instance_map.pb-c.h"
#include "config_map_function.pb-c.h"
#include "config_item.pb-c.h"
#include "config_map.pb-c.h"

int load_all_config();
CFGSCENE *get_scene_config_byid(int32_t scene_id);
int create_all_static_scene();

CFGMAP *get_map_config_byid(int32_t id);
CFGSKILL *get_skill_config_byid(int32_t skill_id);
CFGDROP *get_drop_config_byid(int32_t drop_id);
CFGPLAYER *get_player_config_byid(int32_t prof, int32_t level);
CFGINSTANCEMAP *get_instance_map_config_byid(int32_t id);
CFGMAPFUNCTION *get_instance_function_byid(int32_t id);

CFGMAINQUEST *get_quest_main_config_byid(int32_t id);

CFGATTRIBUTE *get_attribute_config_byid(int32_t id);

CFGNPC *get_npc_config_byid(int32_t id);
ATTRIBUTE *get_npc_attr_byid(int32_t id);

CFGITEM *get_item_config_byid(uint32_t id);

//初始化NPC的属性
void reset_npc_attr(int32_t id);
#endif

