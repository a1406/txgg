#include "handle_conn_msg.h"
#include "server_proto.h"
#include "game_srv.h"
#include "game_net.h"
#include "cs.pb-c.h"
#include "err.pb-c.h"
#include "role.h"
#include "scene.h"
#include "special_proto.h"
#include "game_rand.h"
#include "gm.h"
#include "quest.h"
#include "config.h"
#include "friend.h"
#include "keyword.pb-c.h"

int check_login_resp(PROTO_HEAD_CONN *head)
{
	LoginResp *login_req = NULL;

	login_req = login_resp__unpack(NULL, htons(head->len) - sizeof(PROTO_HEAD_CONN), (const uint8_t *)head->data);
	login_req = login_req;
		
	return (0);
}

static int send_fetch_role_request(uint64_t id, CLIENT_MAP *roledb)
{
	assert(roledb);
	PROTO_HEAD *head;
	FETCH_ROLE_DATA_REQUEST *req;
	char sendbuf[sizeof(PROTO_HEAD) + sizeof(FETCH_ROLE_DATA_REQUEST)];
	
	head = (PROTO_HEAD *)&sendbuf[0];
	req = (FETCH_ROLE_DATA_REQUEST *)&sendbuf[sizeof(PROTO_HEAD)];
	head->len = htons(sizeof(sendbuf));
	head->msg_id = htons(SERVER_PROTO_FETCH_ROLE_REQUEST);
	req->id = id;
	send_one_msg(roledb->fd, head, 0);
	return (0);
}

static int handle_login_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret;
	LoginResp msg;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	size_t size;	
	ROLE *role = NULL;
	LoginReq *login_req = NULL;
	uint64_t role_id;
	login_req = login_req__unpack(NULL, len, data);
	if (!login_req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: %d uppack login req failed[%d]", __FUNCTION__, session, len);		
		return (-1);
	}

	role_id = login_req->role_id | ((uint64_t)login_req->area_id << 32);
	role = get_role_by_id(role_id);
	if (role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: kick role[%u][%lu] because the same id login", __FUNCTION__, role->session_id, role->role_id);					
		if (kick_role(role, 1) != 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: role[%lu] logout failed", __FUNCTION__, role->role_id);			
			return (-5);
		}
	}
	
	role = create_role();
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: create role failed", __FUNCTION__);
		ret = -10;
		goto done;
	}
	init_role_data(role);
//	role->fd = fd;
	role->role_id = login_req->role_id;
//	role->conn_srv_index = (active_conn_srv - &conn_server[0]);
	role->session_id = session;
	
	if (add_one_role(role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: add one role [%u] failed", __FUNCTION__, session);
		ret = -20;
		goto done;
	}

	//暂不检查账号
	send_fetch_role_request(role->role_id, active_roledb_srv);
	
//	send_login_resp(conn_server, head->fd);
	ret = 0;
done:	
	if (login_req)	
		login_req__free_unpacked(login_req, NULL);
	if (ret != 0) {
			//send fail resp
		login_resp__init(&msg);
		msg.result = ret;
		size = login_resp__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
		size += sizeof(PROTO_HEAD_CONN);
		head->msg_id = htons(CS__MESSAGE__ID__LOGIN_RESPONSE);
		head->len = htons(size);
		head->fd = ROLE_FD(role);
	
		if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: %d send login resp failed", __FUNCTION__, session);		
		}
		if (role)
			destory_role(role);
	}
	return (ret);
}

static int handle_logout_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	ROLE *role;

	role = get_role_by_session(session);

	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: find role [%u] failed", __FUNCTION__, session);		
		return (-10);		
	}

	if (role_logout(role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%lu] logout failed", __FUNCTION__, role->role_id);
		return (-20);
	}

	return (0);
}

static int send_createrole_request(CreateroleReq *req, CLIENT_MAP *roledb)
{
	assert(req);
	assert(roledb);
	CREATE_ROLE_REQUEST *send_req;
	PROTO_HEAD *head;
	char sendbuf[sizeof(PROTO_HEAD) + sizeof(CREATE_ROLE_REQUEST)];

	head = (PROTO_HEAD *)&sendbuf[0];
	send_req = (CREATE_ROLE_REQUEST *)&sendbuf[sizeof(PROTO_HEAD)];
	head->len = htons(sizeof(sendbuf));
	head->msg_id = htons(SERVER_PROTO_CREATE_ROLE_REQUEST);
	send_req->id = req->role_id;
	send_req->profession = req->profession;
	strncpy(send_req->name, req->name, sizeof(send_req->name));
	send_req->name[sizeof(send_req->name) - 1] = '\0';
	send_one_msg(roledb->fd, head, 0);
	return (0);
}

static int handle_createrole_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	CreateroleResp msg;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	size_t size;	
	int ret = 0;
	ROLE *role = NULL;	
	CreateroleReq *req = NULL;
	req = createrole_req__unpack(NULL, len, data);

	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: uppack create role req failed[%d]", __FUNCTION__, len);		
		return (-1);		
	}

	if (!req->name || req->name[0] == '\0') {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: create role req invalid [%d]", __FUNCTION__, len);		
		return (-10);				
	}

	role = get_role_by_session(session);
	if (!role) {
		role = create_role();
		if (!role) {
			log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
				"%s: create role failed[%u]", __FUNCTION__, session);
			ret = -20;
			goto done;
		}
//		role->fd = fd;
//		role->conn_srv_index = (active_conn_srv - &conn_server[0]);
		role->session_id = session;
		role->role_id = req->role_id;
//		role->one_role[0].attr.name[0] = '\0';
		init_role_data(role);		
		if (add_one_role(role) != 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
				"%s: add one role failed[%u][%lu]", __FUNCTION__, session, role->role_id);
			destory_role(role);
			ret = -30;
			goto done;
		}
	}
	if (role->one_role[0].attr.name[0] != '\0' || role->role_id != req->role_id) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: security check failed[%u][%lu]", __FUNCTION__, session, role->role_id);
		ret = -40;
		goto done;
	}
	send_createrole_request(req, active_roledb_srv);
	ret = 0;
done:	
	if (req)	
		createrole_req__free_unpacked(req, NULL);
	if (ret != 0) {
		createrole_resp__init(&msg);
		msg.result = ret;
		size = createrole_resp__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
		size += sizeof(PROTO_HEAD_CONN);
		head->msg_id = htons(CS__MESSAGE__ID__CREATEROLE_RESPONSE);
		head->len = htons(size);
		head->fd = ROLE_FD(role);
	
		if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: %d send login resp failed", __FUNCTION__, session);		
		}
	}
	return (ret);
}

static int handle_move_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret;
	MoveReq *req = NULL;
	ROLE *role;
	
	req = move_req__unpack(NULL, len, data);

	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: uppack create role req failed[%d]", __FUNCTION__, len);		
		return (-1);		
	}

	role = get_role_by_session(session);
	if (!role) {
		ret = -10;
		goto done;
	}

	role->path.begin.pos_x = req->move_start_x;
	role->path.begin.pos_y = req->move_start_y;
	role->path.end.pos_x = req->move_end_x;
	role->path.end.pos_y = req->move_end_y;
	broadcast_move_notify(role);

	ret = 0;
done:	
	if (req)	
		move_req__free_unpacked(req, NULL);	
	return (ret);
}

static int send_gm_resp(ROLE *role, uint32_t result)
{
	GmResponse msg;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	assert(role);

	gm_response__init(&msg);
	msg.result = result;
	
	size = gm_response__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__GM_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (0);
}

static int send_chat_resp(ROLE *role, ChatRequest *req)
{
	ChatResponse msg;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	assert(role);
	assert(req);	

	chat_response__init(&msg);
	msg.result = 0;
	msg.channel = req->channel;
	msg.target_role_id = req->target_role_id;
	msg.target_area_id = req->target_area_id;
	msg.target_name = "not set";
	msg.msg = req->msg;
	msg.pic_index = req->pic_index;
	msg.pic_name = req->pic_name;	
	
	size = chat_response__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__CHAT_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (0);
}

static int send_chat_notify(ROLE *role, ChatRequest *req)
{
	int i;
	ChatNotify msg;
	size_t size;
	PROTO_HEAD *real_head;
	PROTO_HEAD_CONN_BROADCAST *head;	
	SCENE *scene;

	assert(role);
	assert(req);	
	
	head = (PROTO_HEAD_CONN_BROADCAST *)toclient_send_buf;
	real_head = &head->real_head;
	
	scene = role->scene.p;
	if (unlikely(!scene)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: find scene failed[%u] [%lu]", __FUNCTION__, ROLE_FD(role), role->role_id);
		return (-10);

	}

	chat_notify__init(&msg);
	msg.channel = req->channel;
	msg.src_role_id = role->role_id & 0xffffffff;
	msg.src_area_id = role->role_id >> 32;
	msg.src_name = role->one_role[0].attr.name;
	msg.dst_role_id = req->target_role_id;
	msg.dst_area_id = req->target_area_id;
	msg.dst_name = "not set";
	msg.msg = req->msg;
	msg.pic_index = req->pic_index;
	msg.pic_name = req->pic_name;
	
	size = chat_notify__pack(&msg, (uint8_t *)real_head->data);
	real_head->len = htons(sizeof(PROTO_HEAD) + size);
	real_head->msg_id = htons(CS__MESSAGE__ID__CHAT_NOTIFY);

	for (i = 0; i < max_conn_srv; ++i)
		broadcast_to_scene_except(scene, role->session_id, head, i);	
	return (0);
}

static int send_transport_resp(ROLE *role, int scene_id, int ret)
{
	TransportResp msg;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;

	transport_resp__init(&msg);
	msg.result = ret;
	msg.scene_id = scene_id;
	msg.pos_x = role->path.end.pos_x;
	msg.pos_y = role->path.end.pos_y;		
	
	size = transport_resp__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__TRANSPORT_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send transport resp to %lu failed", __FUNCTION__, role->role_id);		
	}
	
	return (0);
}

static int send_enter_instance_resp(ROLE* role, int result, uint32_t id)
{
	EnterInstanceResp msg;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;

	if (!role)
		return (0);
	
	enter_instance_resp__init(&msg);
	size = enter_instance_resp__get_packed_size(&msg);

	msg.result = result;
	msg.instance_id = id;

	size = enter_instance_resp__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__ENTER_INSTANCE_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send enter instance resp to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (0);
}

static int send_leave_instance_resp(ROLE *role, uint32_t scene_id)
{
	LeaveInstanceResp msg;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;

	leave_instance_resp__init(&msg);
	size = leave_instance_resp__get_packed_size(&msg);
	size += sizeof(PROTO_HEAD_CONN);

	msg.scene_id = scene_id;
	msg.pos_x = role->path.end.pos_x;
	msg.pos_y = role->path.end.pos_y;
	
	size = leave_instance_resp__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)])) + sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__LEAVE_INSTANCE_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send enter instance resp to %lu failed", __FUNCTION__, role->role_id);		
	}	
	return (0);
}

static int handle_drop_container_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	size_t size;	
	DropContainerItemRequest *req = NULL;
	DropContainerItemResponse resp;
	CONTAINER *container;
	ROLE *role;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;	
	role = get_role_by_session(session);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		ret = -10;
		goto done;
	}

	req = drop_container_item_request__unpack(NULL, len, data);
	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack list container failed by role[%u][%lu] failed", __FUNCTION__, session, role->role_id);			
		ret = -15;
		goto done;
	}

	switch (req->container_type)
	{
		case CONTAINER__TYPE__BAG:
			container = (CONTAINER *)role->bag.p;
			break;
		case CONTAINER__TYPE__STORE:
			container = (CONTAINER *)role->store.p;			
			break;
		default:
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: invalid container type  by role[%u][%lu]", __FUNCTION__, session, role->role_id);			
			ret = -20;
			goto done;			
			
	}

	drop_container_item_response__init(&resp);

	//response 里面有信息了 不用发notify了 
	resp.result = delete_thing_bypos(container, req->thing_pos, req->thing_num);
	resp.container_type = req->container_type;
	resp.thing_pos = req->thing_pos;
	resp.thing_num = req->thing_num;	
	
	size = drop_container_item_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__DROP_CONTAINER_ITEM_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send drop container resp to %lu failed", __FUNCTION__, role->role_id);		
	}
	on_bag_changed(role);
	
done:
	if (req) {
		drop_container_item_request__free_unpacked(req, NULL);
	}
	return (ret);
}

static int handle_equip_change_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	size_t size;	
	EquipChangeRequest *req = NULL;
	EquipChangeResponse resp;
	CONTAINER *container;
	uint32_t equip_id, thing_id = 0;
	THING_OBJ *thing;	
	ROLE *role;
	CFGITEM *config;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;

	role = get_role_by_session(session);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return -10;
	}

	equip_change_response__init(&resp);

	req = equip_change_request__unpack(NULL, len, data);
	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack failed by role[%u][%lu] failed", __FUNCTION__, session, role->role_id);			
		ret = -20;
		goto done;
	}
	container = (CONTAINER *)role->bag.p;
	thing = get_thingobj_bypos(container, req->bag_pos);
	equip_id = get_role_equip_id(role, req->friend_pos, req->equip_pos);

	if (thing) {
		config = get_item_config_byid(thing->id);		
		if (thing->num > 1 || !config || config->type != req->equip_pos) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: role[%u][%lu] thing[%u] equip_pos[%d] invalid",
				__FUNCTION__, session, role->role_id, thing->id, req->equip_pos);
			ret = -30;
			goto done;
		}
	}
	
	if (thing) {
		thing_id = thing->id;
		if (equip_id)
			thing->id = equip_id;
		else
			delete_thing_bypos(container, req->bag_pos, 1);
	} else if (equip_id) {
		add_thing_bypos(container, req->bag_pos, equip_id, 1);
	}
	
	set_role_equip_id(role, req->friend_pos, req->equip_pos, thing_id);

done:
		//todo check param
	resp.result = ret;
	if (req) {
		resp.friend_pos = req->friend_pos;	
		resp.equip_pos = req->equip_pos;
		resp.bag_pos = req->bag_pos;
	}
	size = equip_change_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__EQUIP_CHANGE_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send swap container resp to %lu failed", __FUNCTION__, role->role_id);		
	}	
	if (req) {
		equip_change_request__free_unpacked(req, NULL);
	}
	return (ret);
}

static int handle_swap_container_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	size_t size;	
	SwapContainerItemRequest *req = NULL;
	SwapContainerItemResponse resp;
	CONTAINER *container;
	ROLE *role;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;

	role = get_role_by_session(session);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		ret = -10;
		goto done;
	}

	req = swap_container_item_request__unpack(NULL, len, data);
	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack list container failed by role[%u][%lu] failed", __FUNCTION__, session, role->role_id);			
		ret = -15;
		goto done;
	}
	switch (req->container_type)
	{
		case CONTAINER__TYPE__BAG:
			container = (CONTAINER *)role->bag.p;
			break;
		case CONTAINER__TYPE__STORE:
			container = (CONTAINER *)role->store.p;			
			break;
		default:
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: invalid container type  by role[%u][%lu]", __FUNCTION__, session, role->role_id);			
			ret = -20;
			goto done;			
			
	}
	swap_container_item_response__init(&resp);

	resp.result = swap_thing_pos(container, req->thing_pos1, req->thing_pos2);
	resp.container_type = req->container_type;
	resp.thing_pos1 = req->thing_pos1;
	resp.thing_pos2 = req->thing_pos2;
	
	size = swap_container_item_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__SWAP_CONTAINER_ITEM_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send swap container resp to %lu failed", __FUNCTION__, role->role_id);		
	}	
done:
	if (req) {
		swap_container_item_request__free_unpacked(req, NULL);
	}
	return (ret);
}

static int handle_list_container_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int i, j;
	size_t size;
	int ret = 0;
	ContainerThing *point[MAX_THING_OBJ_PER_CONTAINER];
	ContainerThing things[MAX_THING_OBJ_PER_CONTAINER];
	ListContainerItemRequest *req = NULL;
	ListContainerItemResponse resp;
	CONTAINER *container;
	ROLE *role;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	
	role = get_role_by_session(session);
	if (!role || role->one_role[0].attr.name[0] == '\0') {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		ret = -10;
		goto done;
	}

	req = list_container_item_request__unpack(NULL, len, data);
	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack list container failed by role[%u][%lu] failed", __FUNCTION__, session, role->role_id);			
		ret = -15;
		goto done;
	}
	switch (req->container_type)
	{
		case CONTAINER__TYPE__BAG:
			container = (CONTAINER *)role->bag.p;
			break;
		case CONTAINER__TYPE__STORE:
			container = (CONTAINER *)role->store.p;			
			break;
		default:
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: invalid container type  by role[%u][%lu]", __FUNCTION__, session, role->role_id);			
			ret = -20;
			goto done;			
			
	}

	if (!container) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%u][%lu] do not have container %d failed", __FUNCTION__, session, role->role_id, req->container_type);					
		ret = -30;
		goto done;
	}

	list_container_item_response__init(&resp);
	resp.result = 0;
	resp.max_thing_num = container->max_thing_num;
	resp.container_type = req->container_type;
	resp.things = point;
	for (i = 0, j = 0; i < container->max_thing_num; ++i) {
		if (!container->things[i].p)
			continue;
		point[j] = &things[j];
		container_thing__init(&things[j]);
		things[j].thing_pos = i;
		things[j].thing_id = ((THING_OBJ *)container->things[i].p)->id;
		things[j].thing_num = ((THING_OBJ *)container->things[i].p)->num;
		++j;
	}
	resp.n_things = j;
	size = list_container_item_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__LIST_CONTAINER_ITEM_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send list container resp to %lu failed", __FUNCTION__, role->role_id);		
	}
	
done:
	if (req) {
		list_container_item_request__free_unpacked(req, NULL);
	}
	
	return (ret);
}

static int handle_enter_instance_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	ROLE *role;
	SCENE *scene;
	INSTANCE *instance;
	EnterInstanceReq *req = NULL;

	role = get_role_by_session(session);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		ret = -10;
		goto done;
	}
	
	req = enter_instance_req__unpack(NULL, len, data);
	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack enter instance failed by role[%u][%lu] failed", __FUNCTION__, session, role->role_id);			
		ret = -15;
		goto done;
	}
	
	scene = (SCENE *)role->scene.p;
	if (!scene) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: get scene by role[%u][%lu] failed", __FUNCTION__, session, role->role_id);			
		ret = -20;
		goto done;
	}
	instance = (INSTANCE *)(role->instance.p);
	if (instance) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%u][%lu] already in instance %u", __FUNCTION__, session, role->role_id, instance->id);
		ret = -30;
		goto done;		
	}

	instance = new_instance(req->instance_id);
	if (!instance) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: create instance failed", __FUNCTION__);			
		ret = -50;
		goto done;
	}
	if (enter_instance(role, instance) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role [%u][%lu] enter instance failed", __FUNCTION__, ROLE_FD(role), role->role_id);			
		ret = -60;
		goto done;		
	}
	if (del_role_from_scene(scene, role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: del role[%u][%lu] from scene failed", __FUNCTION__, ROLE_FD(role), role->role_id);			
		ret = -70;
		goto done;		
	}
done:
	if (req) {
		send_enter_instance_resp(role, ret, req->instance_id);
		enter_instance_req__free_unpacked(req, NULL);
	}

	return ret;
}

static int handle_leave_instance_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;	
	ROLE *role;
	SCENE *scene;
	INSTANCE *instance;	
	role = get_role_by_session(session);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		ret = -10;
		goto done;
	}
	scene = (SCENE *)role->scene.p;
	if (!scene) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: get scene by role[%u][%lu] failed", __FUNCTION__, session,  role->role_id);
		ret = -20;
		goto done;
	}
	instance = (INSTANCE *)(role->instance.p);
	if (!instance) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: get instance by role[%u][%lu] failed", __FUNCTION__, session,  role->role_id);
		ret = -30;
		goto done;		
	}

	if (leave_instance(role, instance) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role [%u][%lu] leave instance failed", __FUNCTION__, ROLE_FD(role), role->role_id);					
		ret = -40;
		goto done;				
	}

	send_leave_instance_resp(role, scene->id);
	
	if (add_role_to_scene(scene, role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: add role[%u][%lu] to scene failed", __FUNCTION__, ROLE_FD(role), role->role_id);			
		ret = -50;
		goto done;
	}
done:
	return ret;	
}

static int send_rand_dice_response(ROLE *role)
{
	InstanceRandDiceResponse msg;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;

	instance_rand_dice_response__init(&msg);
	msg.num = role->instance_data.dice;
	
	size = instance_rand_dice_response__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__INSTANCE_RAND_DICE_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send resp to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (0);
}

static int handle_rand_dice_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	ROLE *role;
	role = get_role_by_session(session);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-1);
	}
	if (role->instance_data.dice > 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: role[%u][%lu] already diced [%u]", __FUNCTION__, session,  role->role_id, role->instance_data.dice);
//		assert(0);
		return (-10);
	}

	if (role->instance_data.special_dice > 0) {
		role->instance_data.dice = role->instance_data.special_dice;
		role->instance_data.special_dice = 0;
	} else {
		role->instance_data.dice = get_rand_between(1, 6);
	}

	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: %lu rand dice %d", __FUNCTION__, role->role_id, role->instance_data.dice);
	
	send_rand_dice_response(role);
		//instance_rand_dice_response__pack
	return (0);
}

static int send_instance_move_response(ROLE *role, InstanceMoveRequest *req, int32_t result)
{
	InstanceMoveResponse msg;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;

	assert(role);
	
	instance_move_response__init(&msg);
	msg.result = result;
	if (req) {
		msg.n_id = req->n_id;
		msg.id = req->id;
	}

	size = instance_move_response__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);

	head->msg_id = htons(CS__MESSAGE__ID__INSTANCE_MOVE_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send resp to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (0);
}

static struct instance_node *search_instance_node_byid(INSTANCE *instance, uint16_t id)
{
	int i;
	for (i = 0; i < instance->node_num; ++i) {
		if (instance->node[i].id == id)
			return &instance->node[i];
	}
	return NULL;
}

static inline struct instance_node *get_instance_next_node_byid(INSTANCE *instance, struct instance_node *cur_node, uint16_t id)
{
	struct instance_node *ret;
	if (cur_node->up_pos >= 0) {
		ret = &instance->node[cur_node->up_pos];
		if (ret->id == id)
			return ret;
	}
	if (cur_node->down_pos >= 0) {
		ret = &instance->node[cur_node->down_pos];
		if (ret->id == id)
			return ret;
	}
	if (cur_node->left_pos >= 0) {
		ret = &instance->node[cur_node->left_pos];
		if (ret->id == id)
			return ret;
	}
	if (cur_node->right_pos >= 0) {
		ret = &instance->node[cur_node->right_pos];
		if (ret->id == id)
			return ret;
	}
	return NULL;
}

static inline uint8_t get_instance_node_type(int32_t action)
{
	return ((action >> 16) & 0xff);
}

static inline int get_node_path_num(struct instance_node *node)
{
	int ret = 0;
	if (node->up_pos >= 0)
		++ret;
	if (node->down_pos >= 0)
		++ret;
	if (node->left_pos >= 0)
		++ret;
	if (node->right_pos >= 0)
		++ret;
	return ret;
}

static int handle_instance_finish_reward_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	size_t size;
	ROLE *role;
	INSTANCE *instance;
	CFGMAP *config;
	static uint32_t item_id[MAX_THINGLIST_NUM];
	static uint32_t item_num[MAX_THINGLIST_NUM];
	static DropData drop_data;	
	InstanceFinishRewardResponse resp;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;	
	
	role = get_role_by_session(session);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-1);
	}
	instance = (INSTANCE *)role->instance.p;
	if (!instance) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: role[%lu] not in instance, failed", __FUNCTION__, role->role_id);		
		return (-15);
	}
	instance_finish_reward_response__init(&resp);
	drop_data__init(&drop_data);
	drop_data.item_id = item_id;
	drop_data.item_num = item_num;	
	
	resp.drop_data = &drop_data;
	resp.result = 0;
	resp.pass = (role->instance_data.state == INSTANCE_STATE_FINISHED);
	if (resp.pass) {
		config = get_map_config_byid(instance->id);
		if (config) {
			give_role_drop(role, config->award, resp.drop_data);
		}
	}
	
	size = instance_finish_reward_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)])) + sizeof(PROTO_HEAD_CONN);

	head->msg_id = htons(CS__MESSAGE__ID__INSTANCE_FINISH_REWARD_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);		
	}

	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: %lu get reward", __FUNCTION__, role->role_id);
	return (0);
}

static int handle_instance_shortcut_change_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	size_t size;
	ROLE *role;
	INSTANCE *instance;
	InstanceShortcutChangeRequest *req;
	InstanceShortcutChangeResponse resp;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;	
	
	role = get_role_by_session(session);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-1);
	}
	instance = (INSTANCE *)role->instance.p;
	if (!instance) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: role[%lu] not in instance, failed", __FUNCTION__, role->role_id);		
		return (-15);
	}

	req = instance_shortcut_change_request__unpack(NULL, len, data);
	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: role[%lu] uppack failed", __FUNCTION__, role->role_id);		
		return (-20);		
	}
	
	if (req->shortcut_pos < 0 || req->shortcut_pos >= MAX_SHORTCUT_PER_ROLE)
		goto done;
	role->shortcut[req->shortcut_pos].id = req->item_id;
	role->shortcut[req->shortcut_pos].pos = req->item_pos;	
	
	instance_shortcut_change_response__init(&resp);
	resp.result = 0;
	resp.item_id = req->item_id;
	resp.item_pos = req->item_pos;
	resp.shortcut_pos = req->shortcut_pos;
	
	size = instance_shortcut_change_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)])) + sizeof(PROTO_HEAD_CONN);

	head->msg_id = htons(CS__MESSAGE__ID__INSTANCE_SHORTCUT_CHANGE_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);		
	}

done:
	instance_shortcut_change_request__free_unpacked(req, NULL);

	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: %lu use %u[%u] ret %u", __FUNCTION__, role->role_id, req->item_id,
		req->item_pos, req->shortcut_pos, resp.result);

	return (0);
}

static int handle_instance_use_item_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	size_t size;
	ROLE *role;
	INSTANCE *instance;
	InstanceUseItemRequest *req;
	InstanceUseItemResponse resp;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;	
	
	role = get_role_by_session(session);
	if (unlikely(!role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-10);
	}
	instance = (INSTANCE *)role->instance.p;
	if (unlikely(!instance)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: role[%lu] not in instance, failed", __FUNCTION__, role->role_id);		
		return (-20);
	}

	req = instance_use_item_request__unpack(NULL, len, data);
	if (unlikely(!req)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: role[%lu] unpack failed", __FUNCTION__, role->role_id);		
		return (-30);		
	}
	
	instance_use_item_response__init(&resp);
	resp.result = do_instance_use_item(role, req);
	resp.item_id = req->item_id;
	resp.item_param = req->item_param;	
	
	size = instance_use_item_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)])) + sizeof(PROTO_HEAD_CONN);

	head->msg_id = htons(CS__MESSAGE__ID__INSTANCE_USE_ITEM_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);		
	}

	instance_use_item_request__free_unpacked(req, NULL);

	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: %lu use %u[%u] ret %u", __FUNCTION__, role->role_id, req->item_id, req->item_param, resp.result);			
	return (0);
}

static int handle_instance_stop_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	size_t size;
	ROLE *role;
	INSTANCE *instance;	
	InstanceStopResponse resp;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;	
	struct instance_node *node;
	uint8_t type;
	int num_path;
	
	role = get_role_by_session(session);
	if (unlikely(!role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-1);
	}
	instance = (INSTANCE *)role->instance.p;
	if (unlikely(!instance)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: role[%lu] not in instance, failed", __FUNCTION__, role->role_id);		
		return (-15);
	}
	
	node = &instance->node[role->instance_data.seq_id];
	num_path = get_node_path_num(node);
	if (num_path <= 1)
		role->instance_data.dice = 0;
	
	if (role->instance_data.dice != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: role[%lu] instance_dice is[%d], failed", __FUNCTION__, role->role_id, role->instance_data.dice);
		return (-10);		
	}

	instance_stop_response__init(&resp);


	type = get_instance_node_type(node->action);
	if (role->instance_data.nowar_times == 0 && (type == CFG__KEYWORD__NAME__instance_node_type_fight
/*			|| type == CFG__KEYWORD__NAME__instance_node_type_elite */
			|| type == CFG__KEYWORD__NAME__instance_node_type_boss))
		resp.result = CS__ERROR__NO__ERR_INSTANCE_ENTERFIGHT;
	else
		resp.result = CS__ERROR__NO__ERR_SUCCESS;
	
	size = instance_stop_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)])) + sizeof(PROTO_HEAD_CONN);

	head->msg_id = htons(CS__MESSAGE__ID__INSTANCE_STOP_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);		
	}

	do_instance_action(role, type, node);
	
	return (0);
}
	
static int handle_instance_move_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int i;
	int ret = 0;
	INSTANCE *instance;
	InstanceMoveRequest *req = NULL; 
	ROLE *role;
	CFGINSTANCEMAP *map;
	struct instance_node *node;
	uint8_t type;
	
	role = get_role_by_session(session);
	if (unlikely(!role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-1);
	}

	instance = (INSTANCE *)role->instance.p;
	if (unlikely(!instance)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%u][%lu] do not have instance", __FUNCTION__, session,  role->role_id);
		ret = -5;
		goto done;		
	}

	map = get_instance_map_config_byid(instance->id);
	if (unlikely(!map)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%u][%lu] can not find instance[%d] map", __FUNCTION__, session,  role->role_id, instance->id);
		ret = -6;
		goto done;				
	}
	
	req = instance_move_request__unpack(NULL, len, data);
	if (unlikely(!req)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack instance move request by role[%u][%lu] failed", __FUNCTION__, session,  role->role_id);
		ret = -10;
		goto done;
	}
	
	if (req->n_id == 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%u][%lu] dice[%u] != last request[%u]",
			__FUNCTION__, session,  role->role_id, role->instance_data.dice, req->n_id);
		ret = -20;
		goto done;
	}
	//暂不检查方向，所以后退格也不用检查了
	node = &instance->node[role->instance_data.seq_id];
	for (i = 0; i < req->n_id; ++i) {
		type = get_instance_node_type(node->action);
		if (type == CFG__KEYWORD__NAME__instance_node_type_transport) {  //碰到传送格子，清零骰子
			if (i + 1 != req->n_id) {
				log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
					"%s %d: role[%u][%lu] dice[%u] != last request[%u], failed",
					__FUNCTION__, __LINE__, session,  role->role_id, role->instance_data.dice, req->n_id);
				ret = -30;
				goto done;

			}
			
			role->instance_data.dice = req->n_id;
			node = search_instance_node_byid(instance, req->id[i]);
			if (!node) {
				log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
					"%s: role[%u][%lu] can not find transport target[%u]",
					__FUNCTION__, session,  role->role_id, req->id[i]);
				ret = -40;
				goto done;
			}
		} else {
			node = get_instance_next_node_byid(instance, node, req->id[i]);
			if (!node) {
				log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
					"%s: role[%u][%lu] path[%u] wrong",
					__FUNCTION__, session,  role->role_id, req->id[i]);
				ret = -50;
				goto done;
			}
		}
	}

	if (req->n_id > role->instance_data.dice) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s %d: role[%u][%lu] dice[%u] != last request[%u], failed",
			__FUNCTION__, __LINE__, session,  role->role_id, role->instance_data.dice, req->n_id);
		ret = -60;
		goto done;
	}
	
	role->instance_data.seq_id = node->seq_id;
	role->instance_data.action = 1;  //todo
		
done:
	if (ret == 0) {
		role->instance_data.dice -= req->n_id;
	} else {
//		assert(0);
	}
	send_instance_move_response(role, req, ret);
	if (req)
		instance_move_request__free_unpacked(req, NULL);
	return (ret);
}

static int handle_gm_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	ROLE *role;	
	GmRequest *req = NULL;

	role = get_role_by_session(session);
	if (unlikely(!role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		ret = -10;
		goto done;
	}
	req = gm_request__unpack(NULL, len, data);
	if (unlikely(!req)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack by role[%u][%lu] failed", __FUNCTION__, session,  role->role_id);
		ret = -30;
		goto done;
	}
	ret = run_gm_command(role, req->command);
	send_gm_resp(role, ret);
done:
	if (req)	
		gm_request__free_unpacked(req, NULL);

	return (ret);
}

static int handle_chat_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	ROLE *role;	
	ChatRequest *req = NULL;

	role = get_role_by_session(session);
	if (unlikely(!role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		ret = -10;
		goto done;
	}
	req = chat_request__unpack(NULL, len, data);
	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack chat by role[%u][%lu] failed", __FUNCTION__, session,  role->role_id);
		ret = -30;
		goto done;
	}
	send_chat_resp(role, req);
	send_chat_notify(role, req);
done:
	if (req)	
		chat_request__free_unpacked(req, NULL);

	return (ret);
}

int handle_query_role_info_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	QueryRoleInfoRequest *req = NULL;
	QueryRoleInfoResponse resp;
	RoleSightInfo info;
//	uint32_t equip[MAX_FRIEND_PER_ROLE][MAX_EQUIP_PER_ROLE];
	RoleSummary summary[MAX_FRIEND_PER_ROLE];
	RoleSummary *summary_point[MAX_FRIEND_PER_ROLE];
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	size_t size;
	int ret = 0;	
	ROLE *role, *session_role;		
	int i;//, j;
	uint64_t role_id;

	req = query_role_info_request__unpack(NULL, len, data);
	if (unlikely(!req)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack by session[%u] failed", __FUNCTION__, session);
		return (-1);
	}

	session_role = get_role_by_session(session);
	if (unlikely(!session_role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-5);

	}

	query_role_info_response__init(&resp);
	
	role_id = req->role_id | ((uint64_t)req->area_id << 32);
	role = get_role_by_id(role_id);
	if (unlikely(!role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		ret = (-10);
		goto done;
	}
	

//暂时没有宝宝	
	role_summary__init(&summary[0]);
	summary_point[0] = &summary[0];
	resp.role_summary = summary_point;
	resp.n_role_summary = 1;

	role_sight_info__init(&info);
	resp.info = &info;
	
	info.role_id = req->role_id;
	info.area_id = req->area_id;
	info.move_start_x = role->path.begin.pos_x;
	info.move_start_y = role->path.begin.pos_y;
	info.move_end_x = role->path.end.pos_x;
	info.move_end_x = role->path.end.pos_y;
	info.profession = role->one_role[0].profession;
	info.name = role->one_role[0].attr.name;
	info.level = role->one_role[0].attr.level;
	info.move_speed = role->move_speed;

	resp.scene_id = get_role_sceneid(role);

	for (i = 0; i < MAX_FRIEND_PER_ROLE; ++i) {
		if (!is_role_friend_exist(role, i))
			break;

		summary[i].strength = role->one_role[i].strength;
		summary[i].brain = role->one_role[i].brain;
		summary[i].charm = role->one_role[i].charm;
		summary[i].agile = role->one_role[i].agile;

		summary[i].n_equip = MAX_EQUIP_PER_ROLE;
		summary[i].equip = role->one_role[i].equip;
/*		
		summary[i].n_equip = MAX_EQUIP_PER_ROLE;
		summary[i].equip = equip[i];

		for (j = 0; j < MAX_EQUIP_PER_ROLE; ++j)
			equip[i][j] = role->one_role[i].equip[j];
*/
		summary[i].profession = role->one_role[i].profession;		
		summary[i].level = role->one_role[i].attr.level;
		summary[i].name = role->one_role[i].attr.name;
		summary[i].n_skill = role->one_role[i].skill_num;
		summary[i].skill = role->one_role[i].skill;
		summary[i].active_skill1 = role->one_role[i].attr.active_skill[0];
		summary[i].active_skill2 = role->one_role[i].attr.active_skill[1];
		summary[i].active_skill3 = role->one_role[i].attr.active_skill[2];

		summary[i].hp_max = role->one_role[i].attr.hp_cur;
		summary[i].hp_cur = role->one_role[i].attr.hp_cur;
		summary[i].phy_attack = role->one_role[i].attr.phy_attack;
		summary[i].phy_defence = role->one_role[i].attr.phy_defence;
		summary[i].magic_attack = role->one_role[i].attr.magic_attack;
		summary[i].magic_defence = role->one_role[i].attr.magic_defence;
		summary[i].parry_rate = role->one_role[i].attr.parry_rate;
		
		summary[i].counterattack_rate = role->one_role[i].attr.counterattack_rate;
		summary[i].hit_rate = role->one_role[i].attr.hit_rate;
		summary[i].jink_rate = role->one_role[i].attr.jink_rate;
		summary[i].crit_rate = role->one_role[i].attr.crit_rate;
		summary[i].speed = role->one_role[i].attr.speed;
		summary[i].exp = role->one_role[i].exp;
	}
done:
	size = query_role_info_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	head->len = htons(size);
	head->fd = ROLE_FD(session_role);
	head->msg_id = htons(CS__MESSAGE__ID__QUERY_ROLE_INFO_RESPONSE);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, session_role->role_id);		
	}
	
	if (req)
		query_role_info_request__free_unpacked(req, NULL);
	return ret;
}

int handle_skill_set_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int i;
	int ret = 0;
	ROLE *role;	
	SkillSetRequest *req;
	SkillSetResponse resp;
	size_t size;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;	

	role = get_role_by_session(session);
	if (unlikely(!role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-10);

	}
	skill_set_response__init(&resp);
	
	req = skill_set_request__unpack(NULL, len, data);
	if (unlikely(!req)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack failed by role[%u][%lu] failed", __FUNCTION__, session,  role->role_id);
		ret = -20;
		goto done;
	}

	assert(req->role_pos == 0);

	if (req->id != 0 && search_one_role_skill(&role->one_role[0], req->id) < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%u][%lu] param check failed, id[%u]", __FUNCTION__, session,  role->role_id, req->id);
		ret = -30;
		goto done;
	}
	
	if (req->pos >= MAX_ACTIVE_SKILL) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%u][%lu] param check failed, pos[%u]", __FUNCTION__, session,  role->role_id, req->pos);
		ret = -40;
		goto done;
	}

	for (i = 0; i < MAX_ACTIVE_SKILL; ++i) {
		if (role->one_role[0].attr.active_skill[i] == req->id)
			role->one_role[0].attr.active_skill[i] = 0;
	}
	role->one_role[0].attr.active_skill[req->pos] = req->id;
	
	resp.id = req->id;
	resp.pos = req->pos;
done:
	resp.result = ret;
	
	size = skill_set_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	head->len = htons(size);
	head->fd = ROLE_FD(role);
	head->msg_id = htons(CS__MESSAGE__ID__SKILL_SET_RESPONSE);

	if (size != send_one_msg(active_conn_srv->fd, (PROTO_HEAD *)head, 0)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: to %lu failed", __FUNCTION__, role->role_id);		
	}
	
	if (req)	
		skill_set_request__free_unpacked(req, NULL);

	return (ret);
}

static int handle_transport_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	ROLE *role;	
	TransportReq *req = NULL;
	SCENE *scene;
	CFGSCENE *config;//get_scene_config_byid(int32_t scene_id);
	
	role = get_role_by_session(session);
	if (unlikely(!role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		ret = -10;
		goto done;
	}
	req = transport_req__unpack(NULL, len, data);
	if (unlikely(!req)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack transport failed by role[%u][%lu] failed", __FUNCTION__, session, role->role_id);
		ret = -15;
		goto done;
	}
	
	scene = get_scene_by_id(req->target_scene_id);
	if (unlikely(!scene)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: get scene[%d] failed", __FUNCTION__, req->target_scene_id);
		ret = -20;
		goto done;
	}

	config = get_scene_config_byid(req->target_scene_id);
	if (!config) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: can not get scene[%d] config", __FUNCTION__, req->target_scene_id);
		ret = -25;
		goto done;		
	}
	
	role->path.begin.pos_x = role->path.end.pos_x = config->enter_pos_x;
	role->path.begin.pos_y = role->path.end.pos_y = config->enter_pos_y;	
	
done:
	send_transport_resp(role, req->target_scene_id, ret);

	if (ret == 0) {
		if (del_role_from_scene(role->scene.p, role) != 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: del role [%u] [%lu] from scene failed", __FUNCTION__, ROLE_FD(role), role->role_id);
		}

		if (add_role_to_scene(scene, role) != 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: add role [%u] [%lu] to scene %d failed", __FUNCTION__, ROLE_FD(role), role->role_id, scene->id);
		}
	}
	
	if (req)	
		transport_req__free_unpacked(req, NULL);

	return (ret);
}

typedef int (*func_handle_connsrv_req)(uint32_t session, const uint8_t *data, uint16_t len);
static func_handle_connsrv_req handle_connsrv_req[CS__MESSAGE__ID__MAX_MSG_ID] =
{
	[CS__MESSAGE__ID__LOGIN_REQUEST] = handle_login_request,
	[CS__MESSAGE__ID__LOGOUT_REQUEST] = handle_logout_request,
	[CS__MESSAGE__ID__CREATEROLE_REQUEST] = handle_createrole_request,
	[CS__MESSAGE__ID__MOVE_REQUEST] = handle_move_request,
	[CS__MESSAGE__ID__TRANSPORT_REQUEST] = handle_transport_request,
	[CS__MESSAGE__ID__ENTER_INSTANCE_REQUEST] = handle_enter_instance_request,
	[CS__MESSAGE__ID__LEAVE_INSTANCE_REQUEST] = handle_leave_instance_request,
	[CS__MESSAGE__ID__SWAP_CONTAINER_ITEM_REQUEST] = handle_swap_container_request,
	[CS__MESSAGE__ID__LIST_CONTAINER_ITEM_REQUEST] = handle_list_container_request,
	[CS__MESSAGE__ID__DROP_CONTAINER_ITEM_REQUEST] = handle_drop_container_request,
	[CS__MESSAGE__ID__INSTANCE_RAND_DICE_REQUEST] = handle_rand_dice_request,
	[CS__MESSAGE__ID__INSTANCE_MOVE_REQUEST] = handle_instance_move_request,
	[CS__MESSAGE__ID__INSTANCE_STOP_REQUEST] = handle_instance_stop_request,
	[CS__MESSAGE__ID__INSTANCE_USE_ITEM_REQUEST] = handle_instance_use_item_request,
	[CS__MESSAGE__ID__INSTANCE_SHORTCUT_CHANGE_REQUEST] = handle_instance_shortcut_change_request,
	[CS__MESSAGE__ID__INSTANCE_FINISH_REWARD_REQUEST] = handle_instance_finish_reward_request,
	[CS__MESSAGE__ID__CHAT_REQUEST] = handle_chat_request,
	[CS__MESSAGE__ID__GM_REQUEST] = handle_gm_request,
	[CS__MESSAGE__ID__QUEST_LIST_REQUEST] = handle_quest_list_request,
	[CS__MESSAGE__ID__QUEST_OPERATE_REQUEST] = handle_quest_operate_request,
	[CS__MESSAGE__ID__SKILL_SET_REQUEST] = handle_skill_set_request,
	[CS__MESSAGE__ID__QUERY_ROLE_INFO_REQUEST] = handle_query_role_info_request,
	[CS__MESSAGE__ID__EQUIP_CHANGE_REQUEST] = handle_equip_change_request,
	[CS__MESSAGE__ID__FRIEND_ADD_REQUEST] = handle_friend_add_request,
	[CS__MESSAGE__ID__FRIEND_DEL_REQUEST] = handle_friend_del_request,
	[CS__MESSAGE__ID__FRIEND_ADD_BLACKLIST_REQUEST] = handle_friend_add_blacklist_request,
	[CS__MESSAGE__ID__FRIEND_DEL_BLACKLIST_REQUEST] = handle_friend_del_blacklist_request,		
};

int decode_client_proto(CLIENT_MAP *conn_server, int index)
{
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)conn_server->buf;
	uint16_t len = htons(head->len) - sizeof(PROTO_HEAD_CONN);
	int id = htons(head->msg_id);
	
	if (likely(id > 0 && id < CS__MESSAGE__ID__MAX_MSG_ID && handle_connsrv_req[id] != NULL))
		handle_connsrv_req[id](head->fd | (index << 16), (const uint8_t *)head->data, len);
	else
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get useless msg id[]", __FUNCTION__, htons(head->msg_id));

	return (0);
}
