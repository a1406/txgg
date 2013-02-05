#ifndef _GAMESRV_CONFIG_H__
#define _GAMESRV_CONFIG_H__

#include <stdint.h>
#include "config_scene.pb-c.h"
#include "config_skill.pb-c.h"
#include "config_drop.pb-c.h"
#include "config_player.pb-c.h"
#include "config_instance_map.pb-c.h"

int load_all_config();
CFGSCENE *get_scene_config_byid(int32_t scene_id);
int create_all_static_scene();

CFGSKILL *get_skill_config_byid(int32_t skill_id);
CFGDROP *get_drop_config_byid(int32_t drop_id);
CFGPLAYER *get_player_config_byid(int32_t prof, int32_t level);
CFGINSTANCEMAP *get_instance_map_config_byid(int32_t id);
#endif

