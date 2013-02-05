#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "game_srv.h"
#include "config.h"
#include "sortarray.h"

#define SCENE_CONFIG_FILE "../tools/xls/scene.output"
#define SKILL_CONFIG_FILE "../tools/xls/skill.output"
#define DROP_CONFIG_FILE "../tools/xls/drop.output"
#define PLAYER_CONFIG_FILE "../tools/xls/player.output"
#define INSTANCE_MAP_CONFIG_FILE "../tools/xls/instance_map.output"

static char config_buf[1024 * 1024];

#define MAX_SCENE_CONFIG (100)
static int num_scene_configs;
static CFGSCENE *all_scene_config[MAX_SCENE_CONFIG];

#define MAX_SKILL_CONFIG (256)
static CFGSKILL *all_skill_config[MAX_SKILL_CONFIG];

#define MAX_DROP_CONFIG (256)
static CFGDROP *all_drop_config[MAX_DROP_CONFIG];

#define MAX_PLAYER_PROF_CONFIG (8)
#define MAX_PLAYER_LEVEL_CONFIG (256)
static CFGPLAYER *all_player_config[MAX_PLAYER_PROF_CONFIG][MAX_PLAYER_LEVEL_CONFIG];

#define MAX_INSTANCE_MAP_CONFIG (1)
static CFGINSTANCEMAP *all_instance_map_config[MAX_INSTANCE_MAP_CONFIG];

CFGINSTANCEMAP *get_instance_map_config_byid(int32_t id)
{
	return all_instance_map_config[0];
}

CFGPLAYER *get_player_config_byid(int32_t prof, int32_t level)
{
	if (unlikely(prof < 0 || prof >= MAX_PLAYER_PROF_CONFIG))
		return NULL;
	if (unlikely(level < 0 || level >= MAX_PLAYER_LEVEL_CONFIG))
		return NULL;		
	return all_player_config[prof][level];
}

CFGDROP *get_drop_config_byid(int32_t drop_id)
{
	if (unlikely(drop_id < 0 || drop_id >= MAX_DROP_CONFIG))
		return NULL;
	return all_drop_config[drop_id];
}

CFGSKILL *get_skill_config_byid(int32_t skill_id)
{
	if (unlikely(skill_id < 0 || skill_id >= MAX_SKILL_CONFIG))
		return NULL;
	return all_skill_config[skill_id];
}

int create_all_static_scene()
{
	struct scene_struct *scene;
	int i;
	for (i = 0; i < num_scene_configs; ++i) {
		GAME_CIRCLEQ_FOREACH(scene, pool_scene_used, base.list_used) {
			if (scene->id == all_scene_config[i]->id)
				break;
		}
		if (scene == (const void *)pool_scene_used) {
			scene = create_scene();
			if (!scene) {
				log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
					"%s: create scene fail", __FUNCTION__);				
				return (-1);
			}
			scene->id = all_scene_config[i]->id;
		}
	}
	return (0);
}

int comp_scene_config(const void *a, const void *b)
{
	if ((*(CFGSCENE **)a)->id == (*(CFGSCENE **)b)->id)
		return (0);
	if ((*(CFGSCENE **)a)->id > (*(CFGSCENE **)b)->id)
		return (1);
	return (-1);
}

CFGSCENE *get_scene_config_byid(int32_t scene_id)
{
	int find, index;
	CFGSCENE data;
	data.id = scene_id;
	index = array_bsearch(&data, all_scene_config, num_scene_configs, sizeof(void *), &find, comp_scene_config);

	if (find == 0)
		return NULL;
	assert(index < num_scene_configs);
	return all_scene_config[index];
}

static int load_all_player_config()
{
	CFGPLAYERS *player_config;
	int size;
	int i;
	int fd = open(PLAYER_CONFIG_FILE, O_RDONLY);
	if (fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: open player config fail[%d]", __FUNCTION__, errno);
		return (-1);		
	}
	size = read(fd, config_buf, sizeof(config_buf));
	if (size < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: read player config fail[%d]", __FUNCTION__, errno);
		return (-10);				
	}
	player_config = cfg__players__unpack(NULL, size, (uint8_t *)config_buf);
	if (!player_config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack player config fail", __FUNCTION__);
		return (-20);						
	}

	for (i = 0; i < player_config->n_player; ++i) {
		if (player_config->player[i]->prof < 0 || player_config->player[i]->prof >= MAX_PLAYER_PROF_CONFIG)
			continue;
		if (player_config->player[i]->level < 0 || player_config->player[i]->level >= MAX_PLAYER_LEVEL_CONFIG)
			continue;		
		all_player_config[player_config->player[i]->prof][player_config->player[i]->level] = player_config->player[i];
	}

	return (0);
}

static int load_all_skill_config()
{
	CFGSKILLS *skill_config;
	int size;
	int i;
	int fd = open(SKILL_CONFIG_FILE, O_RDONLY);
	if (fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: open skill config fail[%d]", __FUNCTION__, errno);
		return (-1);		
	}
	size = read(fd, config_buf, sizeof(config_buf));
	if (size < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: read skill config fail[%d]", __FUNCTION__, errno);
		return (-10);				
	}
	skill_config = cfg__skills__unpack(NULL, size, (uint8_t *)config_buf);
	if (!skill_config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack skill config fail", __FUNCTION__);
		return (-20);						
	}

	for (i = 0; i < skill_config->n_skill; ++i) {
		if (skill_config->skill[i]->id < 0 || skill_config->skill[i]->id >= MAX_SKILL_CONFIG)
			continue;
		all_skill_config[skill_config->skill[i]->id] = skill_config->skill[i];
	}
	return (0);
}

static int load_all_drop_config()
{
	CFGDROPS *drop_config;
	int size;
	int i;
	int fd = open(DROP_CONFIG_FILE, O_RDONLY);
	if (fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: open drop config fail[%d]", __FUNCTION__, errno);
		return (-1);		
	}
	size = read(fd, config_buf, sizeof(config_buf));
	if (size < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: read drop config fail[%d]", __FUNCTION__, errno);
		return (-10);				
	}
	drop_config = cfg__drops__unpack(NULL, size, (uint8_t *)config_buf);
	if (!drop_config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack drop config fail", __FUNCTION__);
		return (-20);						
	}

	for (i = 0; i < drop_config->n_drop; ++i) {
		if (drop_config->drop[i]->id < 0 || drop_config->drop[i]->id >= MAX_DROP_CONFIG)
			continue;
		all_drop_config[drop_config->drop[i]->id] = drop_config->drop[i];
	}
	return (0);
}

static int load_all_scene()
{
	CFGSCENES *scene_config;	
	int size;
	int i;
	int fd = open(SCENE_CONFIG_FILE, O_RDONLY);
	if (fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: open scene config fail[%d]", __FUNCTION__, errno);
		return (-1);		
	}
	size = read(fd, config_buf, sizeof(config_buf));
	if (size < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: read scene config fail[%d]", __FUNCTION__, errno);
		return (-10);				
	}
	scene_config = cfg__scenes__unpack(NULL, size, (uint8_t *)config_buf);
	if (!scene_config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack scene config fail", __FUNCTION__);
		return (-20);						
	}

	for (i = 0; i < scene_config->n_scene; ++i) {
		if (scene_config->scene[i]->id < 0)
			continue;
		if (array_insert(&scene_config->scene[i], all_scene_config, &num_scene_configs, sizeof(void *), 1, comp_scene_config) < 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: load scene config[%d] fail", __FUNCTION__, scene_config->scene[i]->id);			
//			return (-30);
		}
		
	}
	return (0);
}

static int load_instance_map_config()
{
	CFGINSTANCEMAP *config;	
	int size;
	int fd = open(INSTANCE_MAP_CONFIG_FILE, O_RDONLY);
	if (fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: open config fail[%d]", __FUNCTION__, errno);
		return (-1);		
	}
	size = read(fd, config_buf, sizeof(config_buf));
	if (size < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: read  config fail[%d]", __FUNCTION__, errno);
		return (-10);				
	}
	config = cfg__instance__map__unpack(NULL, size, (uint8_t *)config_buf);
	if (!config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack  config fail", __FUNCTION__);
		return (-20);						
	}

	all_instance_map_config[0] = config;
	return (0);
}

int load_all_config()
{
	int ret;
	ret = load_all_scene();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: load scene config fail", __FUNCTION__);
		return (ret);								
	}
	ret = load_all_skill_config();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: load skill config fail", __FUNCTION__);
		return (ret);								
	}

	ret = load_all_drop_config();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: load drop config fail", __FUNCTION__);
		return (ret);								
	}

	ret = load_all_player_config();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: load player config fail", __FUNCTION__);
		return (ret);								
	}

	ret = load_instance_map_config();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: load instance map config fail", __FUNCTION__);
		return (ret);								
	}
	return (0);
}
