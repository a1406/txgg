#ifndef _SCRIPT_DEFINE_H__
#define _SCRIPT_DEFINE_H__

enum function_type
{
	TYPE1_INSTANCE,
	TYPE1_NPC,
	TYPE1_QUEST,
	TYPE1_SCENE,
	MAX_FUNCTION_TYPE	
};

char *type_name[] = {[TYPE1_INSTANCE] = "INSTANCE",
					 [TYPE1_NPC] = "NPC",
					 [TYPE1_QUEST] = "QUEST",
					 [TYPE1_SCENE] = "SCENE"};

enum function_action
{
	INSTANCE_ROLE_ENTER,
	INSTANCE_START,
	INSTANCE_ROLE_LEAVE,
	INSTANCE_END,

	MONSTER_DEAD,
	ROLE_DEAD,

	ITEM_USE,

	NPC_LIST_MENU,
	NPC_CLICK_MENU,	

	TRANSPORT_PRE,
	TRANSPORT_AFTER,	
	
	QUEST_ACCEPT,
	QUEST_COMPLETE,
	QUEST_CANCEL,
	MAX_FUNCTION_ACTION
};

char *action_name[] = {	[INSTANCE_ROLE_ENTER] = "OnRoleEnter",
						[INSTANCE_START] = "OnStart",
						[INSTANCE_ROLE_LEAVE] = "OnRoleLeave",
						[INSTANCE_END] = "OnEnd",

						[MONSTER_DEAD] = "OnMonsterDead",
						[ROLE_DEAD] = "OnRoleDead",

						[ITEM_USE] = "OnItemUse",

						[NPC_LIST_MENU] = "OnNpcListMenu",
						[NPC_CLICK_MENU] = "OnNpcClickMenu",

						[TRANSPORT_PRE] = "OnTransportPre",
						[TRANSPORT_AFTER] = "OnTransportAfter",	
	
						[QUEST_ACCEPT] = "OnQuestAccept",
						[QUEST_COMPLETE] = "OnQuestComplete",
						[QUEST_CANCEL] = "OnQuestCancel"};

union function_param
{
	struct {
		uint8_t type;
		uint32_t param_type;
		uint8_t action;
		uint32_t param_action;
	} param;
	uint64_t data[2];
};

typedef int (*func_execute_script)(const uint8_t type,
                   const uint32_t param_type,
                   const uint8_t action,
                   const uint32_t param_action);


typedef int (*func_script)();

typedef struct registed_function_data
{
	union function_param param;
	func_script func;
} REGISTED_FUNCTION_DATA;

#endif
