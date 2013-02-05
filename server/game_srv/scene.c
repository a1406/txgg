#include <assert.h>
#include <errno.h>
#include "scene.h"
#include "game_srv.h"
#include "game_net.h"
#include "sortarray.h"

void init_scene_data(SCENE *scene)
{
	assert(scene);
	scene->num_roles = 0;
}

int resume_scene_data(SCENE *scene)
{
	assert(scene);	
	int i;
	for (i = 0; i < scene->num_roles; ++i) {
		assert(scene->roles[i].idx != MAX_IDX);
		scene->roles[i].p = (void *)get_role_byidx(scene->roles[i].idx);
		if (!scene->roles[i].p) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: fail", __FUNCTION__);			
			return (-1);
		}
	}
	return (0);
}

//todo  以后不用遍历，用id做下标或者建立索引
SCENE *get_scene_by_id(uint16_t id)
{
	SCENE *scene;
	GAME_CIRCLEQ_FOREACH(scene, pool_scene_used, base.list_used) {
		if (scene->id == id)
			return scene;
	}
	return (NULL);
}

int refresh_scene_struct(SCENE *scene)
{
	assert(scene);
	int i;
	ROLE *role;
	struct timeval now;
	for (i = 0; i < scene->num_roles; ++i) {
		role = scene->roles[i].p;
		assert(role);
		assert(base);
		event_base_gettimeofday_cached(base, &now);
		refresh_path(&role->path, &role->last_refresh_time, &now, 100);
	}
	return (0);
}

int send_role_info_to_otherrole(SCENE *scene, ROLE *role)
{
	int i;
	size_t size;	
	PROTO_HEAD *real_head;
	RefreshSightNotify notify;
	RoleSightInfo info;
	RoleSightInfo *add_role = &info;
	PROTO_HEAD_CONN_BROADCAST *head;
	head = (PROTO_HEAD_CONN_BROADCAST *)toclient_send_buf;
	real_head = &head->real_head;	

	refresh_sight_notify__init(&notify);

	role_sight_info__init(add_role);
	notify.n_add_role = 1;	
	notify.add_role = &add_role;

	notify.n_leave_role_id = 0;
	notify.n_leave_role_area = 0;

	dump_role_sight_info(role, &info);
	
	size = refresh_sight_notify__get_packed_size(&notify);
	if (TOCLIENT_SEND_BUF_SIZE < size) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: REFRESH_SIGHT_NOTIFY_BUF_SIZE no enough[%d]"
			, __FUNCTION__, size);
		return (0);
	}
	refresh_sight_notify__pack(&notify, (uint8_t *)real_head->data);
	
	real_head->msg_id = htons(CS__MESSAGE__ID__REFRESH_SIGHT_NOTIFY);
	real_head->len = htons(sizeof(PROTO_HEAD) + size);

	for (i = 0; i < max_conn_srv; ++i)
		broadcast_to_scene_except(scene, role->session_id, head, i);	
	return (0);
}

int send_otherrole_info_to_client(SCENE *scene, ROLE *role)
{
	int len, ret;
	int i, j;
	ROLE *tmprole;
	int role_num = scene->num_roles - 1;
	if (role_num <= 0)
		return (0);

	size_t size;	
	PROTO_HEAD_CONN *head;
	RoleSightInfo other_role[role_num];
	RoleSightInfo *other_role_point[role_num];
	RefreshSightNotify notify;
	head = (PROTO_HEAD_CONN *)toclient_send_buf;

	refresh_sight_notify__init(&notify);
	notify.n_leave_role_id = 0;
	notify.n_leave_role_area = 0;	

	notify.add_role = other_role_point;
	for (i = 0, j = 0; i < scene->num_roles; ++i) {
		tmprole = scene->roles[i].p;
		if (tmprole->session_id == role->session_id)
			continue;
		
		role_sight_info__init(&other_role[j]);		
		other_role_point[j] = &other_role[j];

		dump_role_sight_info(tmprole, &other_role[j]);
		++j;
	}
//	assert(role_num == j);
	if (unlikely(role_num != j)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: j = [%d], role_num = [%d]"
			, __FUNCTION__, j, role_num);		
	}
	notify.n_add_role = j;
	
	size = refresh_sight_notify__get_packed_size(&notify);
	if (TOCLIENT_SEND_BUF_SIZE < size) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: REFRESH_SIGHT_NOTIFY_BUF_SIZE no enough[%d]"
			, __FUNCTION__, size);
		return (0);
	}
	refresh_sight_notify__pack(&notify, (uint8_t *)head->data);
	len = size + sizeof(PROTO_HEAD_CONN);
	head->len = htons(len);
	head->msg_id = htons(CS__MESSAGE__ID__REFRESH_SIGHT_NOTIFY);
	head->fd = ROLE_FD(role);
	ret = send_to_role(role, (PROTO_HEAD *)head);
	if (ret != len) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send_one_msg fail len[%d] ret[%d] errno[%d]\n", __FUNCTION__, len, ret, errno);			
	}
	return (0);
}

int add_role_to_scene(SCENE *scene, ROLE *role)
{
	IDX idx;	
	assert(scene);
	assert(role);
	idx.p = role;
	idx.idx = GET_IDX(role);	
	if (scene->num_roles >= MAX_ROLE_PER_SCENE) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: scene[%d] full", __FUNCTION__, __LINE__, scene->id);
		return (-1);
	}
	if (array_insert(&idx, &scene->roles[0], (int *)&scene->num_roles, sizeof(IDX), 1, comp_idx) < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: scene[%d] full", __FUNCTION__, __LINE__, scene->id);
		return (-2);
	}
	set_role_scene(role, scene);
	send_role_info_to_otherrole(scene, role);
	send_otherrole_info_to_client(scene, role);


	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: add role  [%u] [%lu] to scene %u", __FUNCTION__, role->session_id, role->role_id, scene->id);		
	return (0);
}
int del_role_from_scene(SCENE *scene, ROLE *role)
{
	int i;
	size_t size;	
	IDX idx;

	if (!scene)
		return (0);
	
	assert(role);
	idx.p = role;
	idx.idx = GET_IDX(role);
	array_delete(&idx, &scene->roles[0], (int *)&scene->num_roles, sizeof(IDX), (comp_func)comp_idx);

	PROTO_HEAD *real_head;
	RefreshSightNotify notify;

	PROTO_HEAD_CONN_BROADCAST *head;
	head = (PROTO_HEAD_CONN_BROADCAST *)toclient_send_buf;
	
	real_head = &head->real_head;
	
	refresh_sight_notify__init(&notify);

	notify.n_add_role = 0;
	notify.n_leave_role_id = 1;
	notify.n_leave_role_area = 1;

	uint32_t role_id, role_area;
	role_id = role->role_id & 0xffffffff;
	role_area = role->role_id >> 32;
	notify.leave_role_id = &role_id;
	notify.leave_role_area = &role_area;		

	size = refresh_sight_notify__get_packed_size(&notify);
	if (TOCLIENT_SEND_BUF_SIZE < size) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: REFRESH_SIGHT_NOTIFY_BUF_SIZE no enough[%d]"
			, __FUNCTION__, size);
		return (0);
	}
	refresh_sight_notify__pack(&notify, (uint8_t *)real_head->data);
	real_head->len = htons(sizeof(PROTO_HEAD) + size);
	real_head->msg_id = htons(CS__MESSAGE__ID__REFRESH_SIGHT_NOTIFY);

	for (i = 0; i < max_conn_srv; ++i)	
		broadcast_to_scene(scene, head, i);

	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
		"%s: del role  [%u] [%lu] from scene %u", __FUNCTION__, role->session_id, role->role_id, scene->id);			
	return (0);
}

int broadcast_to_scene_except(SCENE *scene, uint32_t except_session, PROTO_HEAD_CONN_BROADCAST *head, uint8_t srv_index)
{
	int ret, len;
	uint16_t fd_offset;
	int i;
	uint8_t broadcast_num;
	PROTO_HEAD *real_head;
	ROLE *other_role;
	uint16_t *fd;	
	
	assert(scene);
	assert(head);

	real_head = &head->real_head;
	fd_offset = htons(real_head->len) + offsetof(PROTO_HEAD_CONN_BROADCAST, real_head);
	fd = (uint16_t *)((char *)head + fd_offset);
	
	for (i = 0, broadcast_num = 0; i < scene->num_roles; ++i) {
		other_role = scene->roles[i].p;
		if (CONN_INDEX(other_role) != srv_index)
			continue;
		if (other_role->session_id == except_session)
			continue;
		fd[broadcast_num] = ROLE_FD(other_role);
		++broadcast_num;
	}
	if (broadcast_num <= 0)
		return (0);
		
	len = fd_offset + sizeof(uint16_t) * broadcast_num;
	head->len = htons(len);
	head->msg_id = SERVER_PROTO_BROADCAST;
	head->num_fd = (broadcast_num);

	ret = send_one_msg(conn_server[srv_index].fd, (PROTO_HEAD *)head, 0);
	if (ret != len) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send_one_msg fail len[%d] ret[%d] errno[%d]\n", __FUNCTION__, len, ret, errno);
	}

	return (0);
}

int broadcast_to_scene(SCENE *scene, PROTO_HEAD_CONN_BROADCAST *head, uint8_t srv_index)
{
	int len, ret;
	uint16_t fd_offset;
	int i;
	uint8_t broadcast_num;
	PROTO_HEAD *real_head;
	ROLE *other_role;
	uint16_t *fd;	
	
	assert(scene);
	assert(head);

	real_head = &head->real_head;
	fd_offset = htons(real_head->len) + offsetof(PROTO_HEAD_CONN_BROADCAST, real_head);
	fd = (uint16_t *)((char *)head + fd_offset);
	
	for (i = 0, broadcast_num = 0; i < scene->num_roles; ++i) {
		other_role = scene->roles[i].p;
		if (CONN_INDEX(other_role) != srv_index)
			continue;
		
		fd[broadcast_num] = ROLE_FD(other_role);
		++broadcast_num;
	}
	if (broadcast_num <= 0)
		return (0);

	len = fd_offset + sizeof(uint16_t) * broadcast_num;
	head->len = htons(len);
	head->msg_id = SERVER_PROTO_BROADCAST;
	head->num_fd = (broadcast_num);

	ret = send_one_msg(conn_server[srv_index].fd, (PROTO_HEAD *)head, 0);
	if (ret != len) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send_one_msg fail len[%d] ret[%d]\n", __FUNCTION__, len, ret);
	}

	return (0);
}

int broadcast_move_notify(ROLE *role)
{
	int i;
	size_t size;
	SCENE *scene;
	MoveNotify notify;
	PROTO_HEAD *real_head;
	PROTO_HEAD_CONN_BROADCAST *head;
	assert(role);

	scene = role->scene.p;
	if (!scene) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: role %lu do not have scene obj", __FUNCTION__, role->role_id);		
		return (0);
	}
	
	head = (PROTO_HEAD_CONN_BROADCAST *)toclient_send_buf;
	real_head = &head->real_head;
	
	move_notify__init(&notify);
	notify.role_id = role->role_id & 0xffffffff;
	notify.area_id = role->role_id >> 32;

	notify.move_start_x = role->path.begin.pos_x;
	notify.move_start_y = role->path.begin.pos_y;
	notify.move_end_x = role->path.end.pos_x;
	notify.move_end_y = role->path.end.pos_y;

	size = move_notify__get_packed_size(&notify);
	if (TOCLIENT_SEND_BUF_SIZE < size) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: REFRESH_SIGHT_NOTIFY_BUF_SIZE no enough[%d]"
			, __FUNCTION__, size);
		return (0);
	}
	move_notify__pack(&notify, (uint8_t *)real_head->data);
	real_head->len = htons(sizeof(PROTO_HEAD) + size);
	real_head->msg_id = htons(CS__MESSAGE__ID__MOVE_NOTIFY);

	for (i = 0; i < max_conn_srv; ++i)	
		broadcast_to_scene_except(scene, role->session_id, head, i);
	return (0);
}
