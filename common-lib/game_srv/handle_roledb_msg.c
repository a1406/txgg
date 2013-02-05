#include <assert.h>
#include "game_srv.h"
#include "game_net.h"
#include "client_map.h"
#include "server_proto.h"
#include "cs.pb-c.h"
#include "container.h"
#include "special_proto.h"

static int init_role_data_from_db(DB_ROLE_INFO *info, ROLE *role)
{
	CONTAINER *bag;
	SCENE *scene;
	assert(info);

	scene = get_scene_by_id(info->scene_id);
	if (!scene) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%lu] scene can not found %u", __FUNCTION__, role->role_id, info->scene_id);
		return (-1);
	}

	bag = create_container();
	set_role_bag(role, bag);

		//for test
	bag->max_thing_num = 36;
	int i;
	THING_OBJ *thing;
	for (i = 0; i < 5; ++i) {
		thing = create_thing_obj();
		thing->id = i + 1;
		thing->num = i + 10;
		add_thing_obj(bag, thing);
	}

	role->profession = info->profession;
	role->level = info->level;
	set_role_scene(role, scene);
	memcpy(role->name, info->name, MAX_ROLE_NAME_LEN);
	role->path.begin.pos_x = info->pos.pos_x;
	role->path.begin.pos_y = info->pos.pos_y;
	role->path.end.pos_x = info->pos.pos_x;
	role->path.end.pos_y = info->pos.pos_y;
	reset_role_attribute(role);
	role->hp_cur = role->hp_max;
	role_login(role);
	return (0);
}

int send_login_resp(FETCH_ROLE_DATA_RESPONSE *fetch_role)
{
	assert(fetch_role);
	ROLE *role;	
	LoginResp msg;
	RoleSummary summary;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	size_t size;
	
	role = find_role_by_roleid(fetch_role->id);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: can not find role %lu", __FUNCTION__, fetch_role->id);
		return (-1);
	}
	
	login_resp__init(&msg);
	role_summary__init(&summary);
	msg.role_summary = &summary;

	msg.result = fetch_role->result;	
	msg.scene_id = fetch_role->info.scene_id;
	summary.profession = fetch_role->info.profession;
	summary.level = fetch_role->info.level;
	summary.name = fetch_role->info.name;
	summary.role_id = fetch_role->id & 0xffffffff;
	summary.area_id = fetch_role->id >> 32;
	summary.move_start_x = fetch_role->info.pos.pos_x;
	summary.move_start_y = fetch_role->info.pos.pos_y;
	summary.move_end_x = fetch_role->info.pos.pos_x;
	summary.move_end_y = fetch_role->info.pos.pos_y;		

	size = login_resp__get_packed_size(&msg);
	size = login_resp__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));

	size += sizeof(PROTO_HEAD_CONN);
	head->msg_id = htons(CS__MESSAGE__ID__LOGIN_RESPONSE);
	head->len = htons(size);
	head->fd = role->fd;
	
	if (size != send_one_msg(conn_server.fd, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send login resp to %lu failed", __FUNCTION__, fetch_role->id);		
	}

	if (fetch_role->result != 0) {
//		role_logout(role);
		return (0);
	}

	init_role_data_from_db(&fetch_role->info, role);
	return (0);
}

int send_createrole_resp(CREATE_ROLE_RESPONSE *create_role)
{
	assert(create_role);
	ROLE *role;	
	CreateroleResp msg;
	RoleSummary summary;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	size_t size;
	SCENE *scene;	

	role = find_role_by_roleid(create_role->id);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: can not find role %lu", __FUNCTION__, create_role->id);
		return (-1);
	}
	
	createrole_resp__init(&msg);
	role_summary__init(&summary);
	msg.role_summary = &summary;

	msg.result = create_role->result;	
	msg.scene_id = create_role->info.scene_id;	
	
	summary.profession = create_role->info.profession;
	summary.level = create_role->info.level;
	summary.name = create_role->info.name;
	summary.role_id = create_role->id & 0xffffffff;
	summary.area_id = create_role->id >> 32;
	summary.move_start_x = create_role->info.pos.pos_x;
	summary.move_start_y = create_role->info.pos.pos_y;
	summary.move_end_x = create_role->info.pos.pos_x;
	summary.move_end_y = create_role->info.pos.pos_y;		

	size = createrole_resp__get_packed_size(&msg);
	size = createrole_resp__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));

	size += sizeof(PROTO_HEAD_CONN);
	head->msg_id = htons(CS__MESSAGE__ID__CREATEROLE_RESPONSE);
	head->len = htons(size);
	head->fd = role->fd;

	if (size != send_one_msg(conn_server.fd, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send login resp to %lu failed", __FUNCTION__, create_role->id);		
	}

	if (create_role->result != 0) {
//		role_logout(role);
		return (0);
	}
	scene = get_scene_by_id(create_role->info.scene_id);
	if (!scene) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%lu] scene can not found %u", __FUNCTION__, create_role->id, create_role->info.scene_id);
		return (-1);
	}
/*	
	role->profession = create_role->profession;
	role->level = create_role->level;
//	role->scene.p = (scene);	
//	role->scene.idx = GET_IDX(scene);
	set_role_scene(role, scene);	
	memcpy(role->name, create_role->name, MAX_ROLE_NAME_LEN);	
	role->path.begin.pos_x = create_role->pos.pos_x;
	role->path.begin.pos_y = create_role->pos.pos_y;
	role->path.end.pos_x = create_role->pos.pos_x;
	role->path.end.pos_y = create_role->pos.pos_y;					

	role_login(role);
*/
	init_role_data_from_db(&create_role->info, role);
	return (0);	
}


int handle_roledb_msg(CLIENT_MAP *roledb_server)
{
	PROTO_HEAD *head;

	CREATE_ROLE_RESPONSE *create_role;
	FETCH_ROLE_DATA_RESPONSE *fetch_role;	
	
	assert(roledb_server);

	head = (PROTO_HEAD *)(&roledb_server->buf[0]);

	switch (htons(head->msg_id))
	{
		case SERVER_PROTO_FETCH_ROLE_RESPONSE:
			fetch_role = (FETCH_ROLE_DATA_RESPONSE *)(&head->data[0]);
			send_login_resp(fetch_role);
			break;
		case SERVER_PROTO_CREATE_ROLE_RESPONSE:
			create_role = (CREATE_ROLE_RESPONSE *)(&head->data[0]);
			send_createrole_resp(create_role);
			break;			
		default:
			log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
				"%s: get useless msg id[]", __FUNCTION__, htons(head->msg_id));
			break;			
	}

	fetch_role = fetch_role;
	create_role = create_role;	
	return (0);
}

int test_send_fetch_role(uint16_t fd)
{
	char sendbuf[sizeof(PROTO_HEAD) + sizeof(FETCH_ROLE_DATA_REQUEST)];
	PROTO_HEAD *head = (PROTO_HEAD *)(&sendbuf[0]);
	FETCH_ROLE_DATA_REQUEST *req = (FETCH_ROLE_DATA_REQUEST *)(&sendbuf[sizeof(PROTO_HEAD)]);	

	head->len = htons(sizeof(sendbuf));
	head->msg_id = htons(SERVER_PROTO_FETCH_ROLE_REQUEST);
	req->id = 8686l;
	send_one_msg(fd, head);
	return (0);
}
