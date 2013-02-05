#include "handle_conn_msg.h"
#include "server_proto.h"
#include "game_srv.h"
#include "game_net.h"
#include "cs.pb-c.h"
#include "role.h"
#include "scene.h"
#include "special_proto.h"
#include "game_rand.h"

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
	send_one_msg(roledb->fd, head);
	return (0);
}

static int handle_login_request(uint16_t fd, const uint8_t *data, uint16_t len)
{
	int ret;
	ROLE *role = NULL;
	LoginReq *login_req = NULL;
	uint64_t role_id;
	login_req = login_req__unpack(NULL, len, data);
	if (!login_req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: uppack login req failed[%d]", __FUNCTION__, len);		
		return (-1);
	}

	role_id = login_req->role_id | ((uint64_t)login_req->area_id << 32);
	role = find_role_by_roleid(role_id);
	if (role) {
			//kick old role
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
	role->fd = fd;
	role->role_id = login_req->role_id;
	
	if (add_one_role(role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: add one role [%u] failed", __FUNCTION__, fd);
		ret = -20;
		goto done;
	}

	//暂不检查账号
	send_fetch_role_request(role->role_id, &roledb_server);
	
//	send_login_resp(conn_server, head->fd);
	ret = 0;
done:	
	if (login_req)	
		login_req__free_unpacked(login_req, NULL);
	if (ret != 0 && role)
		destory_role(role);
	return (ret);
}

static int handle_logout_request(uint16_t fd)
{
	ROLE *role;

	role = find_role_by_fd(fd);

	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: find role [%u] failed", __FUNCTION__, fd);		
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
	send_one_msg(roledb->fd, head);
	return (0);
}

static int handle_createrole_request(uint16_t fd, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	ROLE *role = NULL;	
	CreateroleReq *req = NULL;
	req = createrole_req__unpack(NULL, len, data);

	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: uppack create role req failed[%d]", __FUNCTION__, len);		
		return (-1);		
	}

	role = find_role_by_fd(fd);
	if (!role) {
		role = create_role();
		if (!role) {
			log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
				"%s: create role failed[%u]", __FUNCTION__, fd);
			ret = -2;
			goto done;
		}
		role->fd = fd;
		role->role_id = req->role_id;
		role->name[0] = '\0';
		if (add_one_role(role) != 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
				"%s: add one role failed[%u][%lu]", __FUNCTION__, fd, role->role_id);
			destory_role(role);			
			goto done;
		}
	}
	if (role->name[0] != '\0' || role->role_id != req->role_id) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: security check failed[%u][%lu]", __FUNCTION__, fd, role->role_id);		
		goto done;
	}
	send_createrole_request(req, &roledb_server);
	ret = 0;
done:	
	if (req)	
		createrole_req__free_unpacked(req, NULL);
	return (ret);
}

static int handle_move_request(uint16_t fd, const uint8_t *data, uint16_t len)
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

	role = find_role_by_fd(fd);
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

static int send_transport_resp(ROLE *role,int ret)
{
	TransportResp msg;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;

	transport_resp__init(&msg);
	msg.result = ret;
	
	size = transport_resp__get_packed_size(&msg);
	size = transport_resp__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__TRANSPORT_RESPONSE);
	head->len = htons(size);
	head->fd = role->fd;

	if (size != send_one_msg(conn_server.fd, (PROTO_HEAD *)head)) {
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
	head->fd = role->fd;

	if (size != send_one_msg(conn_server.fd, (PROTO_HEAD *)head)) {
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

	size = leave_instance_resp__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)])) + sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__LEAVE_INSTANCE_RESPONSE);
	head->len = htons(size);
	head->fd = role->fd;

	if (size != send_one_msg(conn_server.fd, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send enter instance resp to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (0);
}

static int handle_list_container_request(uint16_t fd, const uint8_t *data, uint16_t len)
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
	
	role = find_role_by_fd(fd);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, fd);				
		ret = -10;
		goto done;
	}

	req = list_container_item_request__unpack(NULL, len, data);
	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack list container failed by role[%u][%lu] failed", __FUNCTION__, fd, role->role_id);			
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
				"%s: invalid container type  by role[%u][%lu]", __FUNCTION__, fd, role->role_id);			
			ret = -20;
			goto done;			
			
	}
	list_container_item_response__init(&resp);
	resp.result = 0;
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
	head->fd = role->fd;

	if (size != send_one_msg(conn_server.fd, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send list container resp to %lu failed", __FUNCTION__, role->role_id);		
	}
	
done:
	if (req) {
		list_container_item_request__free_unpacked(req, NULL);
	}
	
	return (ret);
}

static int handle_enter_instance_request(uint16_t fd, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	ROLE *role;
	SCENE *scene;
	INSTANCE *instance;
	EnterInstanceReq *req = NULL;

	role = find_role_by_fd(fd);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, fd);				
		ret = -10;
		goto done;
	}
	
	req = enter_instance_req__unpack(NULL, len, data);
	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack enter instance failed by role[%u][%lu] failed", __FUNCTION__, fd, role->role_id);			
		ret = -15;
		goto done;
	}
	
	scene = (SCENE *)role->scene.p;
	if (!scene) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: get scene by role[%u][%lu] failed", __FUNCTION__, fd, role->role_id);			
		ret = -20;
		goto done;
	}
	instance = (INSTANCE *)(role->instance.p);
	if (instance) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%u][%lu] already in instance %u", __FUNCTION__, fd, role->role_id, instance->id);
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
			"%s: role [%u][%lu] enter instance failed", __FUNCTION__, role->fd, role->role_id);			
		ret = -60;
		goto done;		
	}
	if (del_role_from_scene(scene, role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: del role[%u][%lu] from scene failed", __FUNCTION__, role->fd, role->role_id);			
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

static int handle_leave_instance_request(uint16_t fd)
{
	int ret = 0;	
	ROLE *role;
	SCENE *scene;
	INSTANCE *instance;	
	role = find_role_by_fd(fd);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, fd);				
		ret = -10;
		goto done;
	}
	scene = (SCENE *)role->scene.p;
	if (!scene) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: get scene by role[%u][%lu] failed", __FUNCTION__, fd, role->role_id);
		ret = -20;
		goto done;
	}
	instance = (INSTANCE *)(role->instance.p);
	if (!instance) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: get instance by role[%u][%lu] failed", __FUNCTION__, fd, role->role_id);
		ret = -30;
		goto done;		
	}

	if (leave_instance(role, instance) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role [%u][%lu] leave instance failed", __FUNCTION__, role->fd, role->role_id);					
		ret = -40;
		goto done;				
	}

	send_leave_instance_resp(role, scene->id);
	
	if (add_role_to_scene(scene, role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: add role[%u][%lu] to scene failed", __FUNCTION__, role->fd, role->role_id);			
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
	msg.num = role->instance_dice;
	
	size = instance_rand_dice_response__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	
	head->msg_id = htons(CS__MESSAGE__ID__INSTANCE_RAND_DICE_RESPONSE);
	head->len = htons(size);
	head->fd = role->fd;

	if (size != send_one_msg(conn_server.fd, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send transport resp to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (0);
}

static int handle_rand_dice_request(uint16_t fd)
{
	ROLE *role;
	role = find_role_by_fd(fd);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, fd);				
		return (-1);
	}
	if (role->instance_dice > 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: role[%u][%lu] already diced [%u]", __FUNCTION__, fd, role->role_id, role->instance_dice);				
		return (-10);
	}
	role->instance_dice = get_rand_between(1, 6);
	send_rand_dice_response(role);
		//instance_rand_dice_response__pack
	return (0);
}

static int send_instance_move_response(ROLE *role, InstanceMoveRequest *req, int32_t result)
{
	InstanceMoveResponse msg;
	size_t size;	
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;
	instance_move_response__init(&msg);
	msg.result = result;
	msg.n_id = req->n_id;
	msg.id = req->id;

	size = instance_move_response__pack(&msg, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);

	head->msg_id = htons(CS__MESSAGE__ID__INSTANCE_MOVE_RESPONSE);
	head->len = htons(size);
	head->fd = role->fd;

	if (size != send_one_msg(conn_server.fd, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send transport resp to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (0);
}

static int handle_instance_move_request(uint16_t fd, const uint8_t *data, uint16_t len)
{
	int i;
	int ret = 0;
	INSTANCE *instance;
	InstanceMoveRequest *req = NULL; 
	ROLE *role;
	role = find_role_by_fd(fd);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, fd);				
		return (-1);
	}
	req = instance_move_request__unpack(NULL, len, data);
	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack instance move request by role[%u][%lu] failed", __FUNCTION__, fd, role->role_id);
		return (-10);
	}
	
	if (req->n_id == 0 || req->n_id != role->instance_dice) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%u][%lu] dice[%u] != last request[%u]",
			__FUNCTION__, fd, role->role_id, role->instance_dice, req->n_id);
		ret = -20;
		goto done;
	}
	instance = (INSTANCE *)role->instance.p;
	if (!instance) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: role[%u][%lu] do not have instance", __FUNCTION__, fd, role->role_id);
		ret = -30;
		goto done;		
	}
	for (i = 0; i < req->n_id; ++i) {
			//todo check
	}
	role->instance_pos = req->id[req->n_id - 1];
		
done:
	send_instance_move_response(role, req, ret);
	if (req)
		instance_move_request__free_unpacked(req, NULL);
	if (ret == 0)
		role->instance_dice = 0;
	return (ret);
}

static int handle_transport_request(uint16_t fd, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	ROLE *role;	
	TransportReq *req = NULL;
	SCENE *scene;
	
	role = find_role_by_fd(fd);
	if (!role) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, fd);				
		ret = -10;
		goto done;
	}
	req = transport_req__unpack(NULL, len, data);
	if (!req) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack transport failed by role[%u][%lu] failed", __FUNCTION__, fd, role->role_id);
		ret = -15;
		goto done;
	}
	
	send_transport_resp(role, ret);
	
	scene = get_scene_by_id(req->target_scene_id);
	if (!scene) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: get scene[%d] failed", __FUNCTION__, req->target_scene_id);
		ret = -20;
		goto done;
	}
	
	if (del_role_from_scene(role->scene.p, role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: del role [%u] [%lu] from scene failed", __FUNCTION__, role->fd, role->role_id);
		ret = -30;
		goto done;
	}
	
	if (add_role_to_scene(scene, role) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: add role [%u] [%lu] to scene %d failed", __FUNCTION__, role->fd, role->role_id, scene->id);
		ret = -40;
		goto done;
	}
done:
	if (req)	
		transport_req__free_unpacked(req, NULL);

	return (ret);
}

int decode_client_proto(CLIENT_MAP *conn_server)
{
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)conn_server->buf;
	uint16_t len = htons(head->len) - sizeof(PROTO_HEAD_CONN);
	switch (htons(head->msg_id))
	{
		case CS__MESSAGE__ID__LOGIN_REQUEST:
			handle_login_request(head->fd, (const uint8_t *)head->data, len);
			break;
		case CS__MESSAGE__ID__LOGOUT_REQUEST:   //这个其实是conn srv发来的，所以不用pb解码
			handle_logout_request(head->fd);
			break;			
		case CS__MESSAGE__ID__CREATEROLE_REQUEST:
			handle_createrole_request(head->fd, (const uint8_t *)head->data, len);
			break;
		case CS__MESSAGE__ID__MOVE_REQUEST:
			handle_move_request(head->fd, (const uint8_t *)head->data, len);			
			break;
		case CS__MESSAGE__ID__TRANSPORT_REQUEST:	
			handle_transport_request(head->fd, (const uint8_t *)head->data, len);		
			break;
		case CS__MESSAGE__ID__ENTER_INSTANCE_REQUEST:
			handle_enter_instance_request(head->fd, (const uint8_t *)head->data, len);					
			break;
		case CS__MESSAGE__ID__LEAVE_INSTANCE_REQUEST:
			handle_leave_instance_request(head->fd);
			break;
		case CS__MESSAGE__ID__USE_CONTAINER_ITEM_REQUEST:
			break;
		case CS__MESSAGE__ID__SORT_CONTAINER_REQUEST:
			break;
		case CS__MESSAGE__ID__SWAP_CONTAINER_ITEM_REQUEST:
			break;
		case CS__MESSAGE__ID__LIST_CONTAINER_ITEM_REQUEST:
			handle_list_container_request(head->fd, (const uint8_t *)head->data, len);
			break;
		case CS__MESSAGE__ID__DROP_CONTAINER_ITEM_REQUEST:
			break;
		case CS__MESSAGE__ID__INSTANCE_RAND_DICE_REQUEST:
			handle_rand_dice_request(head->fd);
			break;
		case CS__MESSAGE__ID__INSTANCE_MOVE_REQUEST:
			handle_instance_move_request(head->fd, (const uint8_t *)head->data, len);
			break;
		default:
			log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
				"%s: get useless msg id[]", __FUNCTION__, htons(head->msg_id));
			break;
	}

	return (0);
}
