#include <signal.h>
#include <assert.h>
#include <search.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>

#include "log4c.h"
#include "game_event.h"
#include "oper_config.h"
#include "client_map.h"
#include "server_proto.h"
#include "special_proto.h"
#include "game_net.h"
#include "cs.pb-c.h"
#include "deamon.h"

log4c_category_t* mycat = NULL;

#define MAX_CLIENT_MAPS  (4096* 4)
#define MAX_SERVER_MAPS  (1)								  

//暂时一个连接服务器只接一个游戏服务器
//static int gamesrv_index = -1;

static struct rb_root server_map_head = RB_ROOT;
static struct rb_root client_map_head = RB_ROOT;

static void release_server_client_map(CLIENT_MAP *client_map);
static void release_client_client_map(CLIENT_MAP *client_map);

int get_right_connect_node_num(CLIENT_MAP *p);
int get_left_connect_node_num(CLIENT_MAP *p)
{
	CLIENT_MAP *left;
	left = get_next_client_map(p, 1);
	if (!left)
		return 0;
	return (get_left_connect_node_num(left) + get_right_connect_node_num(left) + 1);
}

int get_right_connect_node_num(CLIENT_MAP *p)
{
	CLIENT_MAP *right;
	right = get_next_client_map(p, 0);
	if (!right)
		return 0;
	return (get_left_connect_node_num(right) + get_right_connect_node_num(right) + 1);
}

int get_connect_num()
{
	CLIENT_MAP *p;
	p = get_first_client_map(&client_map_head);
	if (!p)
		return (0);
	return get_left_connect_node_num(p) + get_right_connect_node_num(p) + 1;
}

int conn_cb_client_listen_pre()
{
		//todo check client_map size
	return (0);
}

int conn_cb_client_listen_after(evutil_socket_t fd)
{
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: client connected from fd %u", __FUNCTION__, fd);

	return (0);
}

static int kick_role(CLIENT_MAP *client)
{
	CLIENT_MAP *kick_role;
	PROTO_HEAD_CONN *head_conn;
	head_conn = (PROTO_HEAD_CONN *)(client->buf);

	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: kick role from fd [%u]", __FUNCTION__, (head_conn->fd));	
		//todo send msg to client

	kick_role = get_client_map_by_fd(head_conn->fd, &client_map_head);
	if (kick_role)
		remove_listen_callback_event(kick_role);			
	else
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: can not find client_map from fd %u", __FUNCTION__, head_conn->fd);
	return (0);
}

static int broadcast_to_client(CLIENT_MAP *client)
{
	uint16_t fd_offset;
	uint16_t *fd;
	int i;
	int ret = 0;
	PROTO_HEAD *real_head;
	PROTO_HEAD_CONN_BROADCAST *head = (PROTO_HEAD_CONN_BROADCAST *)(client->buf);
	real_head = &head->real_head;
	fd_offset = htons(real_head->len) + offsetof(PROTO_HEAD_CONN_BROADCAST, real_head);
	fd = (uint16_t *)((char *)head + fd_offset);

	//check len
	if (fd_offset + sizeof(uint16_t) * head->num_fd != htons(head->len)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: broadcast len wrong [%u] [%u]", __FUNCTION__, htons(real_head->len), htons(head->len));
		return (-1);		
	}
	
	for (i = 0; i < head->num_fd; ++i) {
//		if (send(fd[i], real_head, htons(real_head->len), 0) != htons(real_head->len)) {
		if (send_one_msg(fd[i], real_head, 1) != htons(real_head->len)) {		
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: broadcast to client[%u] failed err[%d]", __FUNCTION__, fd[i], errno);
			ret = (-1);
		}
	}
	return (ret);
}

static int transfer_to_client(CLIENT_MAP *client)
{
	uint16_t old_len, old_msgid;
	PROTO_HEAD *head;	
	PROTO_HEAD_CONN *head_conn;
	uint16_t fd;
	assert(client);
	head = (PROTO_HEAD *)(client->buf + sizeof(uint16_t));
	head_conn = (PROTO_HEAD_CONN *)(client->buf);

	if (head_conn->msg_id == SERVER_PROTO_BROADCAST)
		return broadcast_to_client(client);
	else if (htons(head_conn->msg_id) == SERVER_PROTO_KICK_ROLE_NOTIFY)
		return kick_role(client);
	
	fd = head_conn->fd;

	old_len = htons(head_conn->len);
	old_msgid = htons(head_conn->msg_id);
	head->msg_id = head_conn->msg_id;
	head->len = htons(old_len - sizeof(uint16_t));

	if (old_len == 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: len wrong drop all msg", __FUNCTION__);
		return (-1);
	}

//	int ret = send(fd, head, htons(head->len), 0);
	int ret = send_one_msg(fd, head, 1);
	if (ret != htons(head->len)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send to client failed err[%d]", __FUNCTION__, errno);
		return (0);
	}
	head_conn->len = htons(old_len);
	head_conn->msg_id = htons(old_msgid);	
	return (0);
}

#define MAX_CACHED_BUF_SIZE (1024 * 1024 * 10)
static int cached_len;
static char cached_buf[MAX_CACHED_BUF_SIZE];

static int cache_to_gamesrv_buf(PROTO_HEAD *head)
{
	int need_len = htons(head->len);
	
	if (need_len > MAX_CACHED_BUF_SIZE - cached_len)
		return (-1);
	memcpy(&cached_buf[cached_len], head, need_len);
	cached_len += need_len;
	return (0);
}

static int send_all_cached_gamesrv_buf(int fd)
{
	PROTO_HEAD *head;
	int i;
	int len;

	if (cached_len <= 0)
		return (0);
	
	for (i = 0; i < cached_len; i += len) {
		head = (PROTO_HEAD *)&cached_buf[i];
		len = htons(head->len);
		if (send_one_msg(fd, head, 0) != len) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: send to gameserver failed err[%d]", __FUNCTION__, errno);
			cached_len = 0;
			return (-1);
		}
	}
	cached_len = 0;
	return (0);
}

static int transfer_to_gameserver(CLIENT_MAP *client)
{
	uint16_t old_len;
	PROTO_HEAD *head;
	PROTO_HEAD_CONN *head_conn;
	CLIENT_MAP *server;	
	assert(client);
	head = (PROTO_HEAD *)CLIENT_MAP_BUF_HEAD(client);
	head_conn = (PROTO_HEAD_CONN *)(client->buf);
	old_len = htons(head->len);
	
	head_conn->len = htons(old_len + sizeof(uint16_t));
	head_conn->msg_id = head->msg_id;	
	head_conn->fd = client->fd;

	server = get_first_client_map(&server_map_head);
	
	if (unlikely(!server)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: do not have game server connected, cache it", __FUNCTION__);
		if (cache_to_gamesrv_buf((PROTO_HEAD *)head_conn) != 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: cache_to_gamesrv_buf failed", __FUNCTION__);			
		}
		goto done;
	}
	
	if (send_one_msg(server->fd, (PROTO_HEAD *)head_conn, 0) != htons(head_conn->len)) {		
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send to gameserver failed err[%d]", __FUNCTION__, errno);
		goto done;
	}
done:
	head->len = htons(old_len);
	return (0);
}

static int send_logout_request(CLIENT_MAP *client)
{
	PROTO_HEAD_CONN *head;
	LOGOUT_REQUEST *req;
	char sendbuf[sizeof(PROTO_HEAD_CONN) + sizeof(LOGOUT_REQUEST)];
	head = (PROTO_HEAD_CONN *)(&sendbuf[0]);
	req = (LOGOUT_REQUEST *)(&sendbuf[sizeof(PROTO_HEAD_CONN)]);
	head->msg_id = htons(CS__MESSAGE__ID__LOGOUT_REQUEST);
	head->len = htons(sizeof(sendbuf));
	head->fd = client->fd;
	req->reason = 0;
	CLIENT_MAP *server;
	
	server = get_first_client_map(&server_map_head);	
	if (unlikely(!server)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: do not have game server connected", __FUNCTION__);
		if (cache_to_gamesrv_buf((PROTO_HEAD *)head) != 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: cache_to_gamesrv_buf failed", __FUNCTION__);			
		}
		return (0);
	}
	
	if (send_one_msg(server->fd, (PROTO_HEAD *)head, 0) != htons(head->len)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: send to gameserver failed err[%d]", __FUNCTION__, errno);
		return (-1);
	}
	return (0);
}

int conn_cb_client_recv(evutil_socket_t fd)
{
	CLIENT_MAP *client;
	int ret;
	client = get_client_map_by_fd(fd, &client_map_head);
	if (client == NULL) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: can not find client_map from fd %u", __FUNCTION__, fd);
		//del_client_map_by_fd(fd, &client_maps[0], (int *)&num_client_map);		
		return (-1);
	}

	ret = get_one_buf(fd, client);
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: connect closed from fd %u, err = %d", __FUNCTION__, fd, errno);
		send_logout_request(client);
//		del_client_map_by_fd(fd, &client_maps[0], (int *)&num_client_map);		
		return (-1);		
	} else if (ret > 0) {
		return (0);
	}

	for (;;) {
		transfer_to_gameserver(client);		
		ret = remove_one_buf(client);
		assert(ret >= 0);
		if (ret == 0)
			break;
	}
	return (0);
}

static CLIENT_MAP *get_client_client_map(evutil_socket_t fd)
{
	CLIENT_MAP *ret;
	CLIENT_MAP *client;
	client = create_client_map();
	if (!client) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: create_client_map failed[%u]", __FUNCTION__, fd);		
		return NULL;
	}
	client->fd = fd;
	client->server_id = 0;
	client->pos_end = client->pos_begin = sizeof(uint16_t);  // 预留两个字节为  conn_head和proto_head的fd字段
	client->cb_recv = conn_cb_client_recv;
	client->cb_release = release_client_client_map;
	
	ret = add_client_map(client, &client_map_head);
	if (!ret) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: add_client_map failed[%u]", __FUNCTION__, fd);
		destory_client_map(client);
		return NULL;
	}
	assert(ret == client);
	return (client);
}
static void release_client_client_map(CLIENT_MAP *client_map)
{
	if (!client_map)
		return;
	uint16_t fd = client_map->fd;
	
	if (del_client_map_by_fd(fd, &client_map_head) != 0)
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: del_client_map [%u] failed", __FUNCTION__, fd);

	destory_client_map(client_map);
	
	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG, "%s: del_client_map [%u] success", __FUNCTION__, fd);		
//	client_map->fd = 0;
	return;
}

int conn_cb_server_listen_pre()
{
		//todo check client_map size
	return (0);
}

int conn_cb_server_listen_after(evutil_socket_t fd)
{
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: server connected from fd %u", __FUNCTION__, fd);
	send_all_cached_gamesrv_buf(fd);
	return (0);
}

int conn_cb_server_recv(evutil_socket_t fd)
{
	CLIENT_MAP *client;
	int ret;
	client = get_client_map_by_fd(fd, &server_map_head);
	if (client == NULL) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: can not find client_map from fd %u", __FUNCTION__, fd);
//		del_client_map_by_fd(fd, &server_maps[0], (int *)&num_server_map);		
		return (-1);
	}
	ret = get_one_buf(fd, client);
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: connect closed from fd %u", __FUNCTION__, fd);
//		del_client_map_by_fd(fd, &server_maps[0], (int *)&num_server_map);		
		return (-10);		
	}
	else if (ret > 0) {
		return (0);
	}

	for (;;) {
		if (transfer_to_client(client) != 0) {
			//todo close connect and return
			//remove_one_buf(client);
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: transfer_to_client fail", __FUNCTION__);
				//client->pos_end = client->pos_begin;
				//break;
		}
		ret = remove_one_buf(client);
		assert(ret >= 0);
		if (ret == 0)
			break;
	}
	return (0);
}

static CLIENT_MAP *get_server_client_map(evutil_socket_t fd)
{
	CLIENT_MAP *ret;
	static CLIENT_MAP client;

	ret = get_first_client_map(&server_map_head);
	if (ret) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: a server already connected", __FUNCTION__);
		return NULL;
	}
	
	client.fd = fd;
	client.server_id = 0;
	client.pos_end = client.pos_begin = 0;
	client.cb_recv = conn_cb_server_recv;
	client.cb_release = release_server_client_map;

	ret = add_client_map(&client, &server_map_head);
	if (!ret)
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: add_client_map failed[%d]", __FUNCTION__, __LINE__, errno);
	assert(ret == &client);
	return (ret);
}
static void release_server_client_map(CLIENT_MAP *client_map)
{
	if (!client_map)
		return;
	uint16_t fd = client_map->fd;
	
	if (del_client_map_by_fd(fd, &server_map_head) != 0)
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: del_client_map [%u] failed", __FUNCTION__, fd);
	
	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG, "%s: del_client_map [%u] success", __FUNCTION__, fd);	
//	client_map->fd = 0;	
	return;
}

static int init_conn_client_map()
{
	int offset = 0;
	static char buf[sizeof(struct pool_client_map_head) * 2 + sizeof(CLIENT_MAP) * MAX_CLIENT_MAPS];

	pool_client_map_used = (struct pool_client_map_head *)(buf + offset);
	offset += sizeof(*pool_client_map_used);

	pool_client_map_freed = (struct pool_client_map_head *)(buf + offset);
	offset += sizeof(*pool_client_map_freed);

	pool_client_map = (CLIENT_MAP *)(buf + offset);
	offset += sizeof(CLIENT_MAP) * MAX_CLIENT_MAPS;	

	init_game_client_map(MAX_CLIENT_MAPS);
	init_client_map();
	return (0);
}

static LISTEN_CALLBACKS callback_server;
static LISTEN_CALLBACKS callback_client;
int main(int argc, char **argv)
{
	int i;
	int ret = 0;
	FILE *file;
	char *line;
	int16_t port;
	
	ret = log4c_init();
	if (ret != 0) {
		printf("log4c_init failed[%d]\n", ret);
		return (ret);
	}
	mycat = log4c_category_get("six13log.log.app.application1");
	if (!mycat) {
		printf("log4c_category_get(\"six13log.log.app.application1\"); failed\n");
		return (0);
	}
	for (i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-d") == 0) {
			change_to_deamon();
			break;
		}
	}

    log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "conn_srv run");

	if (init_conn_client_map() != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "init client map failed");
		goto done;
	}
	
	ret = game_event_init();
	if (ret != 0)
		goto done;

	callback_server.cb_get_client_map = get_server_client_map;
	callback_server.cb_listen_pre = conn_cb_server_listen_pre;
	callback_server.cb_listen_after = conn_cb_server_listen_after;

	callback_client.cb_get_client_map = get_client_client_map;
	callback_client.cb_listen_pre = conn_cb_client_listen_pre;
	callback_client.cb_listen_after = conn_cb_client_listen_after;	

	file = fopen("../server_info.ini", "r");
	if (!file) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "open server_info.ini failed[%d]", errno);				
		ret = -1;
		goto done;
	}
	line = get_first_key(file, "conn_srv_listen_port");
	port = atoi(get_value(line));
	if (port <= 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "config file wrong, no conn_srv_listen_port");
		ret = -1;
		goto done;
	}
	
	ret = game_add_listen_event(port, &callback_client);
	if (ret != 0)
		goto done;

	line = get_first_key(file, "conn_srv_port");
	port = atoi(get_value(line));
	if (port <= 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "config file wrong, no conn_srv_port");
		ret = -1;
		goto done;
	}
	ret = game_add_listen_event(port, &callback_server);
	if (ret != 0)
		goto done;

	fclose(file);

	struct event event_timer;
	struct timeval t1 = {5, 0};	
	add_timer(t1, &event_timer);
//	event_timer.ev_callback = cb_testtool_timer;
	
	struct timeval t2 = {2, 0};	
	add_timer(t2, NULL);

	struct timeval t3 = {4, 0};	
	add_timer(t3, NULL);	

	if (SIG_ERR == signal(SIGPIPE,SIG_IGN)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "set sigpipe ign failed");		
		return (0);
	}
	
	ret = event_base_loop(base, 0);
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "event_base_loop stoped[%d]", ret);	

	struct timeval tv;
	event_base_gettimeofday_cached(base, &tv);
done:
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "conn_srv stoped[%d]", ret);
	return (ret);
}
