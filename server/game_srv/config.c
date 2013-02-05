#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "game_srv.h"
#include "config.h"
#include "sortarray.h"

#define MAP_CONFIG_FILE "../../config/map.output"
#define SCENE_CONFIG_FILE "../../config/scene.output"
#define SKILL_CONFIG_FILE "../../config/skill.output"
#define DROP_CONFIG_FILE "../../config/drop.output"
#define QUEST_MAIN_CONFIG_FILE "../../config/quest_main.output"
#define PLAYER_CONFIG_FILE "../../config/player.output"
#define ITEM_CONFIG_FILE "../../config/item.output"
#define MAP_FUNCTION_FILE "../../config/mapfunction.output"
#define ATTRIBUTE_CONFIG_FILE "../../config/attribute.output"
#define NPC_CONFIG_FILE "../../config/npc.output"


static char config_buf[1024 * 1024 * 5];

#define MAX_SCENE_CONFIG (32)
//static int num_scene_configs;
static CFGSCENE *all_scene_config[MAX_SCENE_CONFIG];

#define MAX_MAP_CONFIG (256)
static CFGMAP *all_map_config[MAX_MAP_CONFIG];

#define MAX_SKILL_CONFIG (256)
static CFGSKILL *all_skill_config[MAX_SKILL_CONFIG];

#define MAX_DROP_CONFIG (10240)
static CFGDROP *all_drop_config[MAX_DROP_CONFIG];

#define MAX_QUEST_MAIN_CONFIG (1000)
static CFGMAINQUEST *all_quest_main_config[MAX_QUEST_MAIN_CONFIG];

#define MAX_PLAYER_PROF_CONFIG (8)
#define MAX_PLAYER_LEVEL_CONFIG (256)
static CFGPLAYER *all_player_config[MAX_PLAYER_PROF_CONFIG][MAX_PLAYER_LEVEL_CONFIG];

#define MAX_INSTANCE_MAP_CONFIG (128)
static CFGINSTANCEMAP *all_instance_map_config[MAX_INSTANCE_MAP_CONFIG];

#define MAX_ATTRIBUTE_CONFIG (40960)
static CFGATTRIBUTE *all_attribute_config[MAX_ATTRIBUTE_CONFIG];

#define MAX_ITEM_CONFIG (65535)
static CFGITEM *all_item_config[MAX_ITEM_CONFIG];

#define MAX_NPC_CONFIG (10240)
static CFGNPC *all_npc_config[MAX_NPC_CONFIG];
static ATTRIBUTE all_npc_attr[MAX_NPC_CONFIG];

#define MAX_INSTANCE_FUNCTION (1024 * 16)
static int instance_function_num;
static CFGMAPFUNCTION *all_instance_function[MAX_INSTANCE_FUNCTION];

void reset_npc_attr(int32_t id)
{
	ATTRIBUTE *attr;
	CFGNPC *npc_config = get_npc_config_byid(id);
	CFGATTRIBUTE *attr_config;	
	if (unlikely(!npc_config))
		return;
	attr = get_npc_attr_byid(id);
	if (unlikely(!attr))
		return;
	attr_config = get_attribute_config_byid(npc_config->attr);
	if (unlikely(!attr_config))
		return;	
	strcpy(attr->name, npc_config->name);
	attr->hp_cur = attr_config->hp;
	attr->level = 1;  //todo
	attr->phy_attack = attr_config->phy_attack;
	attr->phy_defence = attr_config->phy_defence;
	attr->magic_attack = attr_config->magic_attack;
	attr->magic_defence = attr_config->magic_defence;
	attr->parry_rate = attr_config->parry_rate;
	attr->counterattack_rate = attr_config->countattack_rate;
	attr->hit_rate = attr_config->hit_rate;
	attr->jink_rate = attr_config->jink_rate;
	attr->crit_rate = attr_config->crit_rate;
	attr->speed = attr_config->speed;
	attr->buff_num = 0;
	attr->active_skill[0] = npc_config->skill1;
	attr->active_skill[1] = npc_config->skill2;
	attr->active_skill[2] = npc_config->skill3;
	return;
}

CFGINSTANCEMAP *get_instance_map_config_byid(int32_t id)
{
	if (unlikely(id < 0 || id >= MAX_INSTANCE_MAP_CONFIG))
		return NULL;

	return all_instance_map_config[id];
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

CFGATTRIBUTE *get_attribute_config_byid(int32_t id)
{
	if (unlikely(id < 0 || id >= MAX_ATTRIBUTE_CONFIG))
		return NULL;
	return all_attribute_config[id];
}

ATTRIBUTE *get_npc_attr_byid(int32_t id)
{
	if (unlikely(id < 0 || id >= MAX_NPC_CONFIG))
		return NULL;
	return &all_npc_attr[id];		
}

CFGITEM *get_item_config_byid(uint32_t id)
{
	if (unlikely(id < 0 || id >= MAX_ITEM_CONFIG))
		return NULL;
	return all_item_config[id];		
}

CFGNPC *get_npc_config_byid(int32_t id)
{
	if (unlikely(id < 0 || id >= MAX_NPC_CONFIG))
		return NULL;
	return all_npc_config[id];		
}

CFGMAINQUEST *get_quest_main_config_byid(int32_t id)
{
	if (unlikely(id < 0 || id >= MAX_QUEST_MAIN_CONFIG))
		return NULL;
	return all_quest_main_config[id];	
}

CFGMAP *get_map_config_byid(int32_t id)
{
	if (unlikely(id < 0 || id >= MAX_MAP_CONFIG))
		return NULL;
	return all_map_config[id];	
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
	for (i = 0; i < MAX_SCENE_CONFIG; ++i) {
		if (!all_scene_config[i] || all_scene_config[i]->id <= 0)
			continue;
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
/*
int comp_scene_config(const void *a, const void *b)
{
	if ((*(CFGSCENE **)a)->id == (*(CFGSCENE **)b)->id)
		return (0);
	if ((*(CFGSCENE **)a)->id > (*(CFGSCENE **)b)->id)
		return (1);
	return (-1);
}
*/
CFGSCENE *get_scene_config_byid(int32_t scene_id)
{
	if (unlikely(scene_id < 0 || scene_id >= MAX_SCENE_CONFIG))
		return NULL;
	return all_scene_config[scene_id];	
	
/*
	int find, index;
	CFGSCENE data;
	data.id = scene_id;
	index = array_bsearch(&data, all_scene_config, num_scene_configs, sizeof(void *), &find, comp_scene_config);

	if (find == 0)
		return NULL;
	assert(index < num_scene_configs);
	return all_scene_config[index];
*/	
}

static inline int comp_map_function(const void *a, const void *b)
{
	if ((*(CFGMAPFUNCTION **)a)->id == (*(CFGMAPFUNCTION **)b)->id)
		return (0);
	if ((*(CFGMAPFUNCTION **)a)->id > (*(CFGMAPFUNCTION **)b)->id)
		return (1);
	return (-1);
}

CFGMAPFUNCTION *get_instance_function_byid(int32_t id)
{
	int find, index;
	CFGMAPFUNCTION data;
	CFGMAPFUNCTION *p = &data;	
	data.id = id;
	index = array_bsearch(&p, all_instance_function, instance_function_num, sizeof(void *), &find, comp_map_function);

	if (find == 0)
		return NULL;
	assert(index < instance_function_num);

	return all_instance_function[index];
}

static int load_instance_function()
{
	CFGMAPFUNCTIONS *config;
	int size;
	int i;
	int fd = open(MAP_FUNCTION_FILE, O_RDONLY);
	if (fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: open config fail[%d]", __FUNCTION__, errno);
		return (-1);		
	}
	size = read(fd, config_buf, sizeof(config_buf));
	if (size < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: read config fail[%d]", __FUNCTION__, errno);
		close(fd);						
		return (-10);				
	}
	config = cfg__map__functions__unpack(NULL, size, (uint8_t *)config_buf);
	if (!config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack config fail", __FUNCTION__);
		close(fd);						
		return (-20);						
	}

	if (config->n_map_function > MAX_INSTANCE_FUNCTION)
		config->n_map_function = MAX_INSTANCE_FUNCTION;
	for (i = 0; i < config->n_map_function; ++i) {
		array_insert(&config->map_function[i], all_instance_function, &instance_function_num, sizeof(void *), 1, comp_map_function);
	}
	close(fd);				
	return (0);
}

static int load_item_config()
{
	CFGITEMS *config;
	int size;
	int i;
	int fd = open(ITEM_CONFIG_FILE, O_RDONLY);
	if (fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: open config fail[%d]", __FUNCTION__, errno);
		return (-1);		
	}
	size = read(fd, config_buf, sizeof(config_buf));
	if (size < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: read config fail[%d]", __FUNCTION__, errno);
		close(fd);						
		return (-10);				
	}
	config = cfg__items__unpack(NULL, size, (uint8_t *)config_buf);
	if (!config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack config fail", __FUNCTION__);
		close(fd);						
		return (-20);						
	}

	for (i = 0; i < config->n_item; ++i) {
		if (config->item[i]->id < 0 || config->item[i]->id >= MAX_ITEM_CONFIG)
			continue;
		all_item_config[config->item[i]->id] = config->item[i];
	}
	close(fd);				
	return (0);
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
		close(fd);						
		return (-10);				
	}
	player_config = cfg__players__unpack(NULL, size, (uint8_t *)config_buf);
	if (!player_config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack player config fail", __FUNCTION__);
		close(fd);						
		return (-20);						
	}

	for (i = 0; i < player_config->n_player; ++i) {
		if (player_config->player[i]->prof < 0 || player_config->player[i]->prof >= MAX_PLAYER_PROF_CONFIG)
			continue;
		if (player_config->player[i]->level < 0 || player_config->player[i]->level >= MAX_PLAYER_LEVEL_CONFIG)
			continue;		
		all_player_config[player_config->player[i]->prof][player_config->player[i]->level] = player_config->player[i];
	}
	close(fd);				
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
		close(fd);						
		return (-10);				
	}
	skill_config = cfg__skills__unpack(NULL, size, (uint8_t *)config_buf);
	if (!skill_config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack skill config fail", __FUNCTION__);
		close(fd);						
		return (-20);						
	}

	for (i = 0; i < skill_config->n_skill; ++i) {
		if (skill_config->skill[i]->id < 0 || skill_config->skill[i]->id >= MAX_SKILL_CONFIG)
			continue;
		all_skill_config[skill_config->skill[i]->id] = skill_config->skill[i];
	}
	close(fd);					
	return (0);
}

static int load_all_npc_config()
{
	CFGNPCS *config;
	int size;
	int i;
	int fd = open(NPC_CONFIG_FILE, O_RDONLY);
	if (fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: open  config fail[%d]", __FUNCTION__, errno);
		return (-1);		
	}
	size = read(fd, config_buf, sizeof(config_buf));
	if (size < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: read config fail[%d]", __FUNCTION__, errno);
		close(fd);						
		return (-10);				
	}
	config = cfg__npcs__unpack(NULL, size, (uint8_t *)config_buf);
	if (!config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack config fail", __FUNCTION__);
		close(fd);						
		return (-20);						
	}

	for (i = 0; i < config->n_npc; ++i) {
		if (config->npc[i]->id < 0 || config->npc[i]->id >= MAX_NPC_CONFIG)
			continue;
		all_npc_config[config->npc[i]->id] = config->npc[i];
		reset_npc_attr(config->npc[i]->id);
	}
	close(fd);					
	return (0);
}


static int load_all_quest_main_config()
{
	CFGMAINQUESTS *quest_config;
	int size;
	int i;
	int fd = open(QUEST_MAIN_CONFIG_FILE, O_RDONLY);
	if (fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: open drop config fail[%d]", __FUNCTION__, errno);
		return (-1);		
	}
	size = read(fd, config_buf, sizeof(config_buf));
	if (size < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: read drop config fail[%d]", __FUNCTION__, errno);
		close(fd);						
		return (-10);				
	}
	quest_config = cfg__main__quests__unpack(NULL, size, (uint8_t *)config_buf);
	if (!quest_config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack config fail", __FUNCTION__);
		close(fd);						
		return (-20);						
	}

	for (i = 0; i < quest_config->n_quest; ++i) {
		if (quest_config->quest[i]->id < 0 || quest_config->quest[i]->id >= MAX_QUEST_MAIN_CONFIG)
			continue;
		all_quest_main_config[quest_config->quest[i]->id] = quest_config->quest[i];
	}
	close(fd);					
	return (0);
}

static int load_all_attribute_config()
{
	CFGATTRIBUTES *config;
	int size;
	int i;
	int fd = open(ATTRIBUTE_CONFIG_FILE, O_RDONLY);
	if (fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: open  config fail[%d]", __FUNCTION__, errno);
		return (-1);		
	}
	size = read(fd, config_buf, sizeof(config_buf));
	if (size < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: read drop config fail[%d]", __FUNCTION__, errno);
		close(fd);						
		return (-10);				
	}
	config = cfg__attributes__unpack(NULL, size, (uint8_t *)config_buf);
	if (!config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack config fail", __FUNCTION__);
		close(fd);						
		return (-20);						
	}

	for (i = 0; i < config->n_attribute; ++i) {
		if (config->attribute[i]->id < 0 || config->attribute[i]->id >= MAX_ATTRIBUTE_CONFIG)
			continue;
		all_attribute_config[config->attribute[i]->id] = config->attribute[i];
	}
	close(fd);					
	return (0);
}


static int load_all_drop_config()
{
	CFGDROPS *drop_config;
	int size;
	int i, j;
	int rate = 0;
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
		close(fd);						
		return (-10);				
	}
	drop_config = cfg__drops__unpack(NULL, size, (uint8_t *)config_buf);
	if (!drop_config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack drop config fail", __FUNCTION__);
		close(fd);				
		return (-20);						
	}

	//互斥的掉落把掉率累加，这样计算的时候只需要随机一次就可以了
	for (i = 0; i < drop_config->n_drop; ++i) {
		if (!drop_config->drop[i]->exclude)
			continue;
		rate = 0;
		for (j = 0; j < drop_config->drop[i]->n_item; ++j) {
			rate += drop_config->drop[i]->item[j]->rate;
			drop_config->drop[i]->item[j]->rate = rate;
			if (rate > MAX_DROP_RATE)
				log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
					"%s: drop[%u] rate too much", __FUNCTION__, drop_config->drop[i]->id);
		}
	}

	for (i = 0; i < drop_config->n_drop; ++i) {
		if (drop_config->drop[i]->id < 0 || drop_config->drop[i]->id >= MAX_DROP_CONFIG)
			continue;
		all_drop_config[drop_config->drop[i]->id] = drop_config->drop[i];
	}
	close(fd);			
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
		close(fd);						
		return (-10);				
	}
	scene_config = cfg__scenes__unpack(NULL, size, (uint8_t *)config_buf);
	if (!scene_config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack scene config fail", __FUNCTION__);
		close(fd);		
		return (-20);						
	}

	for (i = 0; i < scene_config->n_scene; ++i) {
		if (scene_config->scene[i]->id < 0 || scene_config->scene[i]->id >= MAX_SCENE_CONFIG)
			continue;
		all_scene_config[scene_config->scene[i]->id] = scene_config->scene[i];
	}
	close(fd);
	return (0);
}

static int load_map_config()
{
	CFGMAPS *config;	
	int size;
	int i, fd;
	fd = open(MAP_CONFIG_FILE, O_RDONLY);
	if (fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: open config fail[%d]", __FUNCTION__, errno);
		return (-1);		
	}
	size = read(fd, config_buf, sizeof(config_buf));
	if (size < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: read config fail[%d]", __FUNCTION__, errno);
		close(fd);						
		return (-10);				
	}
	config = cfg__maps__unpack(NULL, size, (uint8_t *)config_buf);
	if (!config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack config fail", __FUNCTION__);
		close(fd);				
		return (-20);						
	}

	for (i = 0; i < config->n_map; ++i) {
		if (config->map[i]->id < 0 || config->map[i]->id >= MAX_MAP_CONFIG)
			continue;
		all_map_config[config->map[i]->id] = config->map[i];
	}
	close(fd);			
	return (0);
}

static int load_instance_map_config()
{
	CFGINSTANCEMAP *config;	
	int size;
	char path[128];
	int i, fd;

	for (i = 0; i < MAX_INSTANCE_MAP_CONFIG; ++i) {
		sprintf(path, "../../config/%d/instn.evt", i);
	
		fd = open(path, O_RDONLY);
		if (fd < 0) {
			continue;
		}
		size = read(fd, config_buf, sizeof(config_buf));
		if (size < 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: read  config fail[%d]", __FUNCTION__, errno);
			continue;
		}
		config = cfg__instance__map__unpack(NULL, size, (uint8_t *)config_buf);
		if (!config) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: unpack  config fail", __FUNCTION__);
			continue;
		}
		
		all_instance_map_config[i] = config;
		close(fd);
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: load instance[%d] config success", __FUNCTION__, i);		
	}
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

	ret = load_all_attribute_config();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: load attribute config fail", __FUNCTION__);
		return (ret);								
	}

	ret = load_all_npc_config();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: load npc config fail", __FUNCTION__);
		return (ret);								
	}

	ret = load_all_quest_main_config();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: load quest main config fail", __FUNCTION__);
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

	ret = load_item_config();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: load item config fail", __FUNCTION__);
		return (ret);								
	}

	ret = load_map_config();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: load map config fail", __FUNCTION__);
		return (ret);								
	}

	ret = load_instance_map_config();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: load instance map config fail", __FUNCTION__);
		return (ret);								
	}
	
	ret = load_instance_function();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: load instance function fail", __FUNCTION__);
		return (ret);								
	}	
	return (0);
}
