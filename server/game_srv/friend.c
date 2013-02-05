#include "friend.h"

int handle_friend_add_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	ROLE *role;	
	FriendAddRequest *req = NULL;
	FriendAddResponse resp;
	size_t size;
	uint64_t id;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;	
	role = get_role_by_session(session);
	if (unlikely(!role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-1);
	}
	friend_add_response__init(&resp);
	
	req = friend_add_request__unpack(NULL, len, data);
	if (unlikely(!req)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack failed by role[%u][%lu] failed", __FUNCTION__, session, role->role_id);
		ret = -10;
		goto done;
	}
	resp.role_id = req->role_id;
	resp.area_id = req->area_id;

	id = ((uint64_t)req->area_id << 32) | req->role_id;
	ret = add_role_game_friend(role, id);
done:
	if (req)
		friend_add_request__free_unpacked(req, NULL);
	size = friend_add_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	resp.result = ret;
	head->msg_id = htons(CS__MESSAGE__ID__FRIEND_ADD_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (ret);
}

int handle_friend_del_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	ROLE *role;	
	FriendDelRequest *req = NULL;
	FriendDelResponse resp;
	size_t size;
	uint64_t id;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;	
	
	role = get_role_by_session(session);
	if (unlikely(!role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-1);
	}
	friend_del_response__init(&resp);	
	req = friend_del_request__unpack(NULL, len, data);
	if (unlikely(!req)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack failed by role[%u][%lu] failed", __FUNCTION__, session, role->role_id);
		ret = -15;
		goto done;
	}
	resp.role_id = req->role_id;
	resp.area_id = req->area_id;

	id = ((uint64_t)req->area_id << 32) | req->role_id;
	ret = del_role_game_friend(role, id);
	
done:
	if (req)
		friend_del_request__free_unpacked(req, NULL);
	size = friend_del_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	resp.result = ret;
	head->msg_id = htons(CS__MESSAGE__ID__FRIEND_DEL_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (ret);
}
	
int handle_friend_add_blacklist_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	ROLE *role;	
	FriendAddBlacklistRequest *req = NULL;
	FriendAddBlacklistResponse resp;
	size_t size;
	uint64_t id;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;	
	
	role = get_role_by_session(session);
	if (unlikely(!role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-1);
	}
	friend_add_blacklist_response__init(&resp);		
	req = friend_add_blacklist_request__unpack(NULL, len, data);
	if (unlikely(!req)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack failed by role[%u][%lu] failed", __FUNCTION__, session, role->role_id);
		ret = -10;
		goto done;
	}
	resp.role_id = req->role_id;
	resp.area_id = req->area_id;

	id = ((uint64_t)req->area_id << 32) | req->role_id;
	ret = add_role_blacklist(role, id);
	
done:
	if (req)
		friend_add_blacklist_request__free_unpacked(req, NULL);
	size = friend_add_blacklist_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	resp.result = ret;
	head->msg_id = htons(CS__MESSAGE__ID__FRIEND_ADD_BLACKLIST_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (ret);
}
	
int handle_friend_del_blacklist_request(uint32_t session, const uint8_t *data, uint16_t len)
{
	int ret = 0;
	ROLE *role;	
	FriendDelBlacklistRequest *req = NULL;
	FriendDelBlacklistResponse resp;
	size_t size;
	uint64_t id;
	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)toclient_send_buf;	

	role = get_role_by_session(session);
	if (unlikely(!role)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: get role obj failed[%u]", __FUNCTION__, session);				
		return (-1);
	}
	friend_del_blacklist_response__init(&resp);
	req = friend_del_blacklist_request__unpack(NULL, len, data);
	if (unlikely(!req)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: unpack failed by role[%u][%lu] failed", __FUNCTION__, session, role->role_id);
		ret = -10;
		goto done;
	}
	resp.role_id = req->role_id;
	resp.area_id = req->area_id;

	id = ((uint64_t)req->area_id << 32) | req->role_id;
	ret = del_role_blacklist(role, id);
	
done:
	if (req)
		friend_del_blacklist_request__free_unpacked(req, NULL);
	size = friend_del_blacklist_response__pack(&resp, &(toclient_send_buf[sizeof(PROTO_HEAD_CONN)]));
	size += sizeof(PROTO_HEAD_CONN);
	resp.result = ret;
	head->msg_id = htons(CS__MESSAGE__ID__FRIEND_DEL_BLACKLIST_RESPONSE);
	head->len = htons(size);
	head->fd = ROLE_FD(role);

	if (size != send_to_role(role, (PROTO_HEAD *)head)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send to %lu failed", __FUNCTION__, role->role_id);		
	}
	return (ret);
}
	
