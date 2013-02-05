#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include "log4c.h"
#include "client_map.h"
#include "cs.pb-c.h"
#include "server_proto.h"
#include "login.h"

extern log4c_category_t* mycat;

extern uint16_t num_client_map;
extern CLIENT_MAP client_maps[];

void dump_move_notify(MoveNotify *p)
{
	if (!p)
		return;
	printf("%s: role[%u][%u] moves from %d %d to %d %d\n", __FUNCTION__, p->role_id, p->area_id,
		p->move_start_x, p->move_start_y, p->move_end_x, p->move_end_y);
}

void dump_login_resp(LoginResp *p)
{
	int i;
	if (!p)
		return;
	printf("%s: result[%u] id[%u], name[%s], profession[%u], sceneid[%u], pos_x[%d] pos_y[%d]\n",
		__FUNCTION__, p->result, p->role_summary->role_id, p->role_summary->name,
		p->role_summary->profession, p->scene_id, p->role_summary->move_start_x, p->role_summary->move_start_x);


	for (i = 0; i < num_client_map; ++i) {
		send_move_request(client_maps[i].fd, 1000, 500, 200, 800);
	}
}

void dump_refresh_notify(RefreshSightNotify *p)
{
	int i;
	if (!p)
		return;
	printf("=== refresh sight ===\n");
	printf("add role %lu\n", p->n_add_role);
	for (i = 0; i < p->n_add_role; ++i) {
		printf("%u %s\n", p->add_role[i]->role_id, p->add_role[i]->name);
	}

	printf("leave role  %lu\n", p->n_leave_role_id);
	for (i = 0; i < p->n_leave_role_id; ++i) {
		printf("%u ", p->leave_role_id[i]);
	}	
	printf("\n=== refresh sight end ===\n");	
}

void dump_create_resp(CreateroleResp *p)
{
	if (!p)
		return;
	if (p->result != 0)
	{
		printf("%s: login %u failed\n", __FUNCTION__, p->role_summary->role_id);
		return;
	}
	printf("%s: result[%u] id[%u], name[%s], profession[%u], sceneid[%u], pos_x[%d] pos_y[%d]\n",
		__FUNCTION__, p->result, p->role_summary->role_id, p->role_summary->name,
		p->role_summary->profession, p->scene_id, p->role_summary->move_start_x, p->role_summary->move_start_x);
	
}

int decode_server_proto(CLIENT_MAP *server)
{
	assert(server);
	MoveNotify *move_notify = NULL;		
	RefreshSightNotify *refresh_notify = NULL;	
	LoginResp *login_resp = NULL;
	CreateroleResp *create_resp = NULL;
	PROTO_HEAD *head = (PROTO_HEAD *)server->buf;
	uint16_t len = htons(head->len) - sizeof(PROTO_HEAD);
	switch (htons(head->msg_id))
	{
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
		default:
			log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
				"%s: get useless msg id[]", __FUNCTION__, htons(head->msg_id));
			break;
	}

	dump_login_resp(login_resp);
	dump_create_resp(create_resp);
	dump_refresh_notify(refresh_notify);
	dump_move_notify(move_notify);
	return (0);
}
