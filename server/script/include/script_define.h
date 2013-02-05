#ifndef _SCRIPT_DEFINE_H__
#define _SCRIPT_DEFINE_H__

//todo
struct script_callback
{
};


enum function_type
{
	TYPE_INSTANCE,     //在副本中会触发
	TYPE_NPC,          //和NPC交互时候触发
	TYPE_QUEST,        //任务类型会总是触发
	TYPE_SCENE,        //在场景内触发
	TYPE_ITEM,         //物品操作的时候触发
	MAX_FUNCTION_TYPE	
};

#ifdef BUILD_TOOL
char *type_name[] = {[TYPE_INSTANCE] = "INSTANCE",
					 [TYPE_NPC] = "NPC",
					 [TYPE_QUEST] = "QUEST",
					 [TYPE_SCENE] = "SCENE"};
#endif

enum function_action
{
	ROLE_ENTER,
	ROLE_LEAVE,
	
	INSTANCE_START,
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
#ifdef BUILD_TOOL
char *action_name[] = {	[ROLE_ENTER] = "OnRoleEnter",
						[ROLE_LEAVE] = "OnRoleLeave",
						
						[INSTANCE_START] = "OnStart",
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
#endif
union function_param
{
	struct {
		uint32_t type;
		uint32_t param_type;
		uint32_t action;
		uint32_t param_action;
	} param;
	uint64_t data[2];
};

typedef int (*func_execute_script)(const uint8_t type,
                   const uint32_t param_type,
                   const uint8_t action,
                   const uint32_t param_action);



struct script_interface
{
	func_execute_script execute_script;
};

typedef struct script_interface* (*func_load_script_so)(void* buff, const int mode, struct script_callback* callback);


typedef int (*func_script)();

typedef struct registed_function_data
{
	union function_param param;
	func_script func;
} REGISTED_FUNCTION_DATA;

#endif
