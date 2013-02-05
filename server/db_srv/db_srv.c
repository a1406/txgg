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
#include "server_proto.h"
#include "client_map.h"
#include "game_net.h"
#include "mysql_module.h"

#include "db_handle.h"
#include "deamon.h"
#include "db_net.h"

#define MAX_SERVER_MAPS  (1)								  
static struct rb_root server_map_head = RB_ROOT;

log4c_category_t* mycat = NULL;
static void roledb_release_client_map(CLIENT_MAP *client_map);

static int handle_db_msg(CLIENT_MAP *client)
{
	assert(client);
	PROTO_HEAD *head = (PROTO_HEAD *)client->buf;
	switch (htons(head->msg_id))
	{
		case SERVER_PROTO_FETCH_ROLE_REQUEST:
			fetch_role_handle(head, client->fd, htons(head->len));
			break;
		case SERVER_PROTO_CREATE_ROLE_REQUEST:
			create_role_handle(head, client->fd, htons(head->len));
			break;
		case SERVER_PROTO_STORE_ROLE_REQUEST:
			store_role_handle(head, client->fd, htons(head->len));
			break;
		default:
			log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
				"%s: get useless msg id[]", __FUNCTION__, htons(head->msg_id));
			break;
	}
	return (0);
}

int dbsrv_cb_server_listen_pre()
{
		//todo check client_map size
	return (0);
}

int dbsrv_cb_server_listen_after(evutil_socket_t fd)
{
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
		"%s: server connected from fd %u", __FUNCTION__, fd);
	send_all_cached_gamesrv_buf(fd);
	return (0);
}

int dbsrv_cb_server_recv(evutil_socket_t fd)
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
		handle_db_msg(client);
		ret = remove_one_buf(client);
		assert(ret >= 0);
		if (ret == 0)
			break;
	}
	return (0);
}

static CLIENT_MAP *roledb_get_client_map(evutil_socket_t fd)
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
	client->pos_end = client->pos_begin = 0;
	client->cb_recv = dbsrv_cb_server_recv;
	client->cb_release = roledb_release_client_map;

	ret = add_client_map(client, &server_map_head);
	if (!ret)
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: add_client_map failed[%d]", __FUNCTION__, __LINE__, errno);
	assert(ret == client);	
	return (client);
}
static void roledb_release_client_map(CLIENT_MAP *client_map)
{
	if (!client_map)
		return;
	uint16_t fd = client_map->fd;
	if (del_client_map_by_fd(fd, &server_map_head) != 0)
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: del_client_map [%u] failed", __FUNCTION__, fd);

	destory_client_map(client_map);

	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG, "%s: del_client_map [%u] success", __FUNCTION__, fd);			
//	client_map->fd = 0;	
	return;
}

int connect_db()
{
	close_db();
	if (init_db("localhost", 3306, "testdb", "root", "123456") != 0) {   //todo get from config file
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "connect db failed");
		return (-1);
	}
	return (0);
}

static int init_roledb_client_map()
{
	int offset = 0;
	static char buf[sizeof(struct pool_client_map_head) * 2 + sizeof(CLIENT_MAP) * MAX_SERVER_MAPS];
	pool_client_map_used = (struct pool_client_map_head *)(buf + offset);
	offset += sizeof(*pool_client_map_used);

	pool_client_map_freed = (struct pool_client_map_head *)(buf + offset);
	offset += sizeof(*pool_client_map_freed);

	pool_client_map = (CLIENT_MAP *)(buf + offset);
	offset += sizeof(CLIENT_MAP) * MAX_SERVER_MAPS;	

	init_game_client_map(MAX_SERVER_MAPS);
	init_client_map();
	return (0);
}


static LISTEN_CALLBACKS callback_server;
int main(int argc, char **argv)
{
	int i;
	int ret = 0;
	FILE *file;
	char *line;
	int16_t port;	

	callback_server.cb_get_client_map = roledb_get_client_map;
	callback_server.cb_listen_pre = dbsrv_cb_server_listen_pre;
	callback_server.cb_listen_after = dbsrv_cb_server_listen_after;
	
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

	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "db_srv run");

	if (init_roledb_client_map() != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "init_roledb_client_map failed");		
		goto done;
	}
	
	if (connect_db() != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "connect_db failed");				
		goto done;
	}
	
	ret = game_event_init();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "game_event_init failed");				
		goto done;
	}


	file = fopen("../server_info.ini", "r");
	if (!file) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "open server_info.ini failed[%d]", errno);				
		ret = -1;
		goto done;
	}
	line = get_first_key(file, "db_srv_port");
	port = atoi(get_value(line));
	if (port <= 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "config file wrong, no db_srv_port");
		ret = -1;
		goto done;
	}
	
	ret = game_add_listen_event(port, &callback_server);
	if (ret != 0)
		goto done;
	
	ret = event_base_loop(base, 0);
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "event_base_loop stoped[%d]", ret);	

done:
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "conn_srv stoped[%d]", ret);
	return (ret);
}
