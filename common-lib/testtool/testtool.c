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
#include "client_map.h"
#include "server_proto.h"
#include "game_net.h"
#include "cs.pb-c.h"
#include "login.h"
#include "deamon.h"

log4c_category_t* mycat = NULL;

#define MAX_CLIENT_MAPS  (1)
uint16_t num_client_map;
CLIENT_MAP client_maps[MAX_CLIENT_MAPS];

CLIENT_MAP *add_tools_client_map(CLIENT_MAP *client)
{
	memcpy(&client_maps[num_client_map], client, sizeof(CLIENT_MAP));
	++num_client_map;
	return &client_maps[num_client_map - 1];
}

CLIENT_MAP *search_client_map_by_fd(uint16_t fd)
{
	int i;
	for (i = 0; i < num_client_map; ++i) {
		if (client_maps[i].fd == fd)
			return &client_maps[i];
	}
	return NULL;
}

static int testtool_cb_conn_recv(evutil_socket_t fd)
{
	CLIENT_MAP *client;
	int ret;

	client = search_client_map_by_fd(fd);
	if (!client) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: can not find client map by fd %u", __FUNCTION__, fd);
		return (-1);				
	}
	
	ret = get_one_buf(fd, client);
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: connect closed from fd %u", __FUNCTION__, fd);
		return (-2);		
	}
	else if (ret > 0) {
		return (0);
	}

	for (;;) {
		decode_server_proto(client);		
		ret = remove_one_buf(client);
		assert(ret >= 0);
		if (ret == 0)
			break;
	}
	return (0);
}

int main(int argc, char **argv)
{
	int i;
	int ret = 0;
	uint32_t id = 8686;
	
	if (argc > 1) {
		id = atoi(argv[1]);
	}
	
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
	
    log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "testtool run");

	ret = game_event_init();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "game_event_init failed[%d]", ret);		
		goto done;
	}
	
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(BASE_SERVER_PORT + CONN_SRV_CLIENT_DELTA);
	ret = evutil_inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);
	if (ret != 1) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "evutil_inet_pton failed[%d]", ret);		
		goto done;
	}

//	callback_conn.need_free = 0;
//	callback_conn.cb_recv = testtool_cb_conn_recv;
	for (i = 0; i < MAX_CLIENT_MAPS; ++i) {
		client_maps[i].cb_recv = testtool_cb_conn_recv;
		
		ret = game_add_connect_event((struct sockaddr *)&sin, sizeof(sin), &client_maps[i]);
		client_maps[num_client_map].fd = ret;
		if (ret > 0) {
			++num_client_map;
//			add_client_map(&client_maps[num_client_map], &client_maps[0], (int *)&num_client_map, MAX_CLIENT_MAPS);
			log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "connect to conn srv success[%d]", ret);
		} else
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "connect to conn srv failed[%d]", errno);			
	}

	for (i = 0; i < num_client_map; ++i) {
//		send_createrole_request(client_maps[i].fd, id);
		send_login_request(client_maps[i].fd, id);		
	}
	
	ret = event_base_loop(base, 0);
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "event_base_loop stoped[%d]", ret);	

done:
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "testtool stoped[%d]", ret);
	return (ret);
}
