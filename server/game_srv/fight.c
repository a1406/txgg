#include <stdlib.h>
#include <stdio.h>
#include "fight.h"
#include "config.h"
#include "game_rand.h"
#include "game_srv.h"

ROLE *choose_rand_role(ROLE *role)
{
	struct role_struct *tmp;

	if (role->one_role[0].attr.hp_max == 0)
		return NULL;
	
	GAME_CIRCLEQ_FOREACH(tmp, pool_role_used, base.list_used) {	
		if (tmp != role && tmp->one_role[0].attr.hp_max != 0)
			return tmp;
	}         
	return NULL;
}

FightNotify *test_fight(ROLE *a, uint32_t param)
{
	FightNotify *result;
	ATTRIBUTE npc;
	CFGNPC *npc_config;
	CFGATTRIBUTE *attr_config;
	int32_t npc_id;
	if (!a)
		return NULL;

	CFGMAPFUNCTION *func = get_instance_function_byid(param);
	if (!func)
		return NULL;

	npc_id = func->monster1;
	
	npc_config = get_npc_config_byid(npc_id);  //todo
	if (!npc_config)
		return NULL;
	
	attr_config = get_attribute_config_byid(npc_config->attr);
	
	strcpy(npc.name, npc_config->name);
	npc.hp_cur = attr_config->hp;
	npc.hp_max = attr_config->hp;	
	npc.level = npc_config->level;
	npc.phy_attack = attr_config->phy_attack;
	npc.phy_defence = attr_config->phy_defence;
	npc.magic_attack = attr_config->magic_attack;
	npc.magic_defence = attr_config->magic_defence;
	npc.parry_rate = attr_config->parry_rate;
	npc.counterattack_rate = attr_config->countattack_rate;
	npc.hit_rate = attr_config->hit_rate;
	npc.jink_rate = attr_config->jink_rate;
	npc.crit_rate = attr_config->crit_rate;
	npc.speed = attr_config->speed;
	npc.buff_num = 0;
	npc.active_skill[0] = npc_config->skill1;
	npc.active_skill[1] = npc_config->skill2;
	npc.active_skill[2] = npc_config->skill3;
	
	result = count_fight_result(&a->one_role[0].attr, &npc);
/*	
	if (npc_config->n_drop && npc_config->drop)
		give_role_drop(a, npc_config->drop[0], result->drop_data);
*/	
	if (result)
		printf("fight result = %d, round = %lu\n", result->result, result->n_round_data);
	a->one_role[0].attr.hp_cur = a->one_role[0].attr.hp_max;
	return result;
}

uint8_t check_left_first(ATTRIBUTE *a, ATTRIBUTE *b)
{
	uint32_t left_speed, right_speed;
	left_speed = a->speed + get_rand_between(1, 30);
	right_speed = b->speed + get_rand_between(1, 30);
	if (left_speed > right_speed)
		return (1);
	return (0);
}

uint32_t count_hurt(uint32_t attack, uint32_t defence)
{
	double rate;
	uint32_t half_defence = defence >> 1;
	
	rate = (get_rand_between(0, 10) - 5.0) / 100.0 + 1;
	return 	(uint32_t)((attack * 1.05) - half_defence + abs(attack - half_defence)) * rate;
}

uint8_t check_can_hit(int64_t hit_rate, int64_t jink_rate, uint16_t level)
{
	double ret = 0.5;
	double tmp = (hit_rate - jink_rate) / level;
	if (tmp >= -0.3)
		ret = tmp + 0.8;
	if (ret >= 1)
		return 1;

	if (get_rand_between(1, 100) <= ret * 100)
		return 1;
	return (0);
}

uint8_t check_can_parry(uint32_t parry_rate, uint16_t level)
{
	double rate = parry_rate / level * 0.02;
	if (rate >= 1)
		return (1);

	if (get_rand_between(1, 100) <= rate * 100)
		return 1;	
	return (0);
}

uint8_t check_can_counterattack(uint32_t counterattack_rate, uint16_t level)
{
	double rate = counterattack_rate / level * 0.02;
	if (rate >= 1)
		return (1);

	if (get_rand_between(1, 100) <= rate * 100)
		return 1;		
	return (0);
}

uint8_t check_can_crit(uint32_t crit_rate, uint16_t level)
{
	double rate = crit_rate / level * 0.02;
	if (rate >= 1)
		return (1);

	if (get_rand_between(1, 100) <= rate * 100)
		return 1;		
	return (0);
}

//todo
uint32_t choose_skill(ROLE *a, ROLE *b, RoundData *round)
{
	assert(a);
	assert(b);
	assert(round);
	return (0);
}

int count_round_result(ATTRIBUTE *a, ATTRIBUTE *b, RoundData *round)
{
	uint32_t hurt;
	uint32_t attack, defence;
//	round->skill_id = choose_skill(a, b, round);
	round->is_jink = !(check_can_hit(a->hit_rate, b->jink_rate, b->level));
	if (round->is_jink)   //木有命中
		return (0);
    //todo 根据skill_id来决定用那种攻防属性
	attack = a->phy_attack;
	defence = b->phy_defence;
	hurt = count_hurt(attack, defence);
	round->is_parry = check_can_parry(b->parry_rate, a->level);
	if (round->is_parry)
		hurt /= 20;
	round->is_crit = check_can_crit(a->crit_rate, b->level);
	if (round->is_crit)
		hurt = hurt << 1;

	//todo 右边玩家掉血
	if (round->left) {
		round->rattr_id[round->n_rattr_id++] = ATTR__ID__HP_CUR;
		round->rattr_value[round->n_rattr_value++] = -hurt;
	} else {
		round->lattr_id[round->n_lattr_id++] = ATTR__ID__HP_CUR;
		round->lattr_value[round->n_lattr_value++] = -hurt;		
	}

	b->hp_cur -= hurt;
	if (b->hp_cur <= 0)
		return (1);
	
	round->is_counterattack = check_can_counterattack(b->counterattack_rate, a->level);
	if (round->is_counterattack) { //计算反击
		attack = b->phy_attack;
		defence = a->phy_defence;		
		hurt = count_hurt(attack, defence);
		hurt = hurt >> 1;
		round->counterattack_hp_delta = -hurt;

		a->hp_cur -= hurt;
		if (a->hp_cur <= 0)
			return (2);		
	}
		
	return (0);
}

#define MAX_ROUND_PER_FIGHT (1280)
#define MAX_ATTR_CHANGED_PER_ROUND (16)

static uint32_t lattr_id[MAX_ROUND_PER_FIGHT][MAX_ATTR_CHANGED_PER_ROUND];
static int32_t lattr_value[MAX_ROUND_PER_FIGHT][MAX_ATTR_CHANGED_PER_ROUND];
static uint32_t rattr_id[MAX_ROUND_PER_FIGHT][MAX_ATTR_CHANGED_PER_ROUND];
static int32_t rattr_value[MAX_ROUND_PER_FIGHT][MAX_ATTR_CHANGED_PER_ROUND];

static uint32_t lbuff_add[MAX_ROUND_PER_FIGHT];
static uint32_t lbuff_del[MAX_ROUND_PER_FIGHT];
static uint32_t rbuff_add[MAX_ROUND_PER_FIGHT];
static uint32_t rbuff_del[MAX_ROUND_PER_FIGHT];
static uint32_t lbuff_effect_attr_id[MAX_ROUND_PER_FIGHT];
static int32_t lbuff_effect_attr_value[MAX_ROUND_PER_FIGHT];
static uint32_t rbuff_effect_attr_id[MAX_ROUND_PER_FIGHT];
static int32_t rbuff_effect_attr_value[MAX_ROUND_PER_FIGHT];			
	
static RoundData round[MAX_ROUND_PER_FIGHT];
static RoundData *round_point[MAX_ROUND_PER_FIGHT];
void init_round(int i)
{
	round_data__init(&round[i]);
	round[i].lattr_id = lattr_id[i];
	round[i].lattr_value = lattr_value[i];
	round[i].rattr_id = rattr_id[i];
	round[i].rattr_value = rattr_value[i];
	
	round[i].lbuff_add = lbuff_add;
	round[i].lbuff_del = lbuff_del;
	round[i].rbuff_add = rbuff_add;
	round[i].rbuff_del = rbuff_del;
	round[i].lbuff_effect_attr_id = lbuff_effect_attr_id;
	round[i].lbuff_effect_attr_value = lbuff_effect_attr_value;
	round[i].rbuff_effect_attr_id = rbuff_effect_attr_id;
	round[i].rbuff_effect_attr_value = rbuff_effect_attr_value;		
	
}

FightNotify *count_fight_result(ATTRIBUTE *a, ATTRIBUTE *b)
{
	static uint32_t item_id[MAX_THINGLIST_NUM];
	static uint32_t item_num[MAX_THINGLIST_NUM];
	static DropData drop_data;
	
	static FightNotify notify;
	static RivalData left, right;
	int i;
	int ret;
	assert(a);
	assert(b);

	fight_notify__init(&notify);
	rival_data__init(&left);
	rival_data__init(&right);
	drop_data__init(&drop_data);

	left.name = a->name;
	left.res_id = 1;
	left.cur_hp = a->hp_cur;
	left.max_hp = a->hp_max;	
	right.name = b->name;
	right.res_id = 1;
	right.cur_hp = b->hp_cur;
	right.max_hp = b->hp_max;		

	drop_data.item_id = item_id;
	drop_data.item_num = item_num;	
	
	notify.drop_data = &drop_data;
	notify.rival_data_left = &left;
	notify.rival_data_right = &right;	
	notify.round_data = round_point;

	if (unlikely(!round_point[0])) {
		for (i = 0; i < MAX_ROUND_PER_FIGHT; ++i)
			round_point[i] = &round[i];
	}
	
	i = 0;
	notify.n_round_data = 1;
	init_round(i);
//	round_data__init(&round[i]);
	round[i].left = check_left_first(a, b);
	if (round[i].left)
		ret = count_round_result(a, b, &round[i]);
	else
		ret = count_round_result(b, a, &round[i]);

	if (ret != 0)
		goto done;
	for (i = 1; i < MAX_ROUND_PER_FIGHT; ++i) {
		init_round(i);		
		round[i].left = !(round[i - 1].left);

		if (round[i].left)
			ret = count_round_result(a, b, &round[i]);
		else
			ret = count_round_result(b, a, &round[i]);

		++notify.n_round_data;
		if (ret != 0)
			goto done;
//		round_data__init(&round[i]);
	}


		//todo 平局？
	return NULL;
done:
	if (a->hp_cur <= 0)
		notify.result = CS__FIGHT__RESULT__RIGHT_WIN;
	else {
		notify.result = CS__FIGHT__RESULT__LEFT_WIN;
	}
	return &notify;
}
