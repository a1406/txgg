#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include "log4c.h"
#include "client_map.h"
#include "cs.pb-c.h"
#include "server_proto.h"
#include "login.h"
#include "define.h"

extern log4c_category_t* mycat;
extern uint32_t Id_Base;
extern uint32_t Id_Count;
extern testuser **usergroup;

void dump_chat_resp(ChatResponse* p, uint32_t count)
{
    if (!p)
		return;
    chat_response__free_unpacked( p, NULL );

    log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"%s: get chat response of %d and free buf used byunpacked!",
                       __FUNCTION__,  count);
}


void dump_enter_instance_resp(EnterInstanceResp * p, uint32_t count)
{
    if (!p)
		return;
    if (p->result != 0) 
	{
        log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"%s: enter instance_id %d get fail result %d !",
                       __FUNCTION__,  p->instance_id, p->result);
        goto release;
    }
    usergroup[count]->userstate = USER_IN_INSTANCE;
    log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"%s: usergroup[%d] enter instance_id %d get  result %d !",
                       __FUNCTION__,  count, p->instance_id, p->result);
release:
    enter_instance_resp__free_unpacked( p, NULL );
}

void dump_leave_instance_resp(LeaveInstanceResp * p, uint32_t count)
{
    if (!p)
		return;
    log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"%s: leave instance and back to scene %d!",
                       __FUNCTION__,  p->scene_id);
    usergroup[count]->userstate = USER_IN_SCENE;
    leave_instance_resp__free_unpacked( p, NULL );
}

void dump_move_notify(MoveNotify *p)
{
	if (!p)
		return;
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"%s: role[%u][%u] moves from %d %d to %d %d!",
                       __FUNCTION__, p->role_id, p->area_id,
                       p->move_start_x, p->move_start_y, p->move_end_x, p->move_end_y);
    move_notify__free_unpacked( p, NULL );
}

void dump_login_resp(LoginResp *p,uint32_t count)
{       
	if (!p)
		return;
	if (p->result != 0) 
	{
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"%s: login fail %u failed !",
                           __FUNCTION__, p->info->role_id);
		goto release;
	}    
    
	log4c_category_log(mycat, LOG4C_PRIORITY_NOTICE,
                       "%s: result[%u] id[%u], name[%s], profession[%u], sceneid[%u], pos_x[%d] pos_y[%d]!",
                       __FUNCTION__, p->result, p->info->role_id, p->role_summary[0]->name,
                       p->info->profession, p->scene_id, p->info->move_start_x, p->info->move_start_y);

    usergroup[count]->userstate = USER_LOGIN;
    usergroup[ count ]->pos_x   = p->info->move_start_x;
    usergroup[ count ]->pos_y   = p->info->move_start_y;
    log4c_category_log(mycat, LOG4C_PRIORITY_TRACE,"user at usergounp[ %d ] status value is %d",
                       count, usergroup[count]->userstate);
release:
      login_resp__free_unpacked( p, NULL );
}

void dump_refresh_notify(RefreshSightNotify *p)
{
	int i;
	if (!p)
        return;
    
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"=== refresh sight ===!");
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"add role %lu!", p->n_add_role);
	for (i = 0; i < p->n_add_role; ++i)
    {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"%u %s!", p->add_role[i]->role_id, p->add_role[i]->name);
	}

	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"leave role  %lu!", p->n_leave_role_id);
	for (i = 0; i < p->n_leave_role_id; ++i)
    {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"%u ", p->leave_role_id[i]);
	}	
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"!=== refresh sight end ===!");
    refresh_sight_notify__free_unpacked( p, NULL );
}

void dump_create_resp(CreateroleResp *p, uint32_t count)
{        
	if (!p)
        return;
	if (p->result != 0) 
	{
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"%s: createrole fail %u failed !",
                           __FUNCTION__, p->info->role_id);
		goto release;
	}
    
    usergroup[count]->userstate = USER_INIT;

    printf("%s:Create role success at usergounp[ %d ],state %d",
                       __FUNCTION__,
                       count, usergroup[count]->userstate);

release:
    createrole_resp__free_unpacked( p, NULL );
}

int decode_server_proto(CLIENT_MAP *server,uint32_t pos_arry)
{
	assert(server);
	MoveNotify *move_notify = NULL;		
	RefreshSightNotify *refresh_notify = NULL;	
	LoginResp *login_resp = NULL;
	CreateroleResp *create_resp = NULL;
    EnterInstanceResp * enter_instance_resp = NULL;
    LeaveInstanceResp * leave_instance_resp = NULL;
    ChatResponse * chat_response = NULL;

    
	PROTO_HEAD *head = (PROTO_HEAD *)server->buf;
	uint16_t len = htons(head->len) - sizeof(PROTO_HEAD);

	switch (htons(head->msg_id))
	{
    case CS__MESSAGE__ID__ENTER_INSTANCE_RESPONSE:
        enter_instance_resp = enter_instance_resp__unpack(NULL, len,
                                                          (const uint8_t *)head->data);
        break;
    case CS__MESSAGE__ID__LEAVE_INSTANCE_RESPONSE:
        leave_instance_resp = leave_instance_resp__unpack(NULL, len,
                                                          (const uint8_t *)head->data);
        break;
        
    case CS__MESSAGE__ID__LOGIN_RESPONSE:
        login_resp = login_resp__unpack(NULL, len, (const uint8_t *)head->data);
        break;
    case CS__MESSAGE__ID__CREATEROLE_RESPONSE:
        create_resp = createrole_resp__unpack(NULL, len, (const uint8_t *)head->data);
        break;
    case CS__MESSAGE__ID__REFRESH_SIGHT_NOTIFY:
        refresh_notify = refresh_sight_notify__unpack(NULL, len, (const uint8_t *)head->data);
        break;
    case CS__MESSAGE__ID__MOVE_NOTIFY:
        move_notify = move_notify__unpack(NULL, len, (const uint8_t *)head->data);
        break;
    case CS__MESSAGE__ID__CHAT_RESPONSE:
        chat_response = chat_response__unpack(NULL,len,(const uint8_t *)head->data);
        break;
    default:
        log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
                           "%s: get useless msg id[]", __FUNCTION__, htons(head->msg_id));
        break;
	}

	dump_login_resp(login_resp, pos_arry);
	dump_create_resp(create_resp, pos_arry);
	dump_refresh_notify(refresh_notify);
	dump_move_notify(move_notify);
    dump_enter_instance_resp(enter_instance_resp, pos_arry);
    dump_leave_instance_resp(leave_instance_resp, pos_arry);
    dump_chat_resp(chat_response,pos_arry);
    return (0);
}
