#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <limits.h>
#include <errno.h>

#include "log4c.h"
#include "mempool.h"
#include "game_event.h"

#include "oper_config.h"
#include "idx.h"
#include "role.h"
#include "scene.h"
#include "thing_obj.h"
#include "container.h"
#include "role.h"
#include "scene.h"
#include "instance.h"
#include "game_queue.h"
#include "game_srv.h"
#include "game_net.h"
#include "client_map.h"
#include "handle_conn_msg.h"
#include "handle_roledb_msg.h"
#include "config.h"
#include "deamon.h"
#include "fight.h"
#include "gm_imp.h"
#include "script.h"

log4c_category_t* mycat = NULL;
uint8_t toclient_send_buf[TOCLIENT_SEND_BUF_SIZE];

POOL_DEF(container)
POOL_DEF(thing_obj)
POOL_DEF(role)
POOL_DEF(scene)
POOL_DEF(instance)

//const static key_t key = 0x10001020;
static key_t key;

static int count_mem_used()
{
	int ret = 0;
	ret += sizeof(struct pool_container_head) * 2 + sizeof(CONTAINER) * max_container_in_game;
	ret += sizeof(struct pool_thing_obj_head) * 2 + sizeof(THING_OBJ) * max_thing_obj_in_game;
	ret += sizeof(struct pool_role_head) * 2 + sizeof(ROLE) * max_role_in_game;
	ret += sizeof(struct pool_scene_head) * 2 + sizeof(SCENE) * max_scene_in_game;
	ret += sizeof(struct pool_instance_head) * 2 + sizeof(INSTANCE) * max_instance_in_game;		
	return (ret);
}

int alloc_shared_mem(int resume)
{
	int size = count_mem_used();
	int shmid;
	if (resume)
		shmid = shmget(key, size, 0666);
	else
		shmid = shmget(key, size, IPC_CREAT|IPC_EXCL|0666);
	if (shmid < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: shmget fail[%d]", __FUNCTION__, __LINE__, errno);		
		return (-1);
	}
	
	void *mem = shmat(shmid, NULL, 0);
	if (!mem) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: shmat fail[%d]", __FUNCTION__, __LINE__, errno);		
		return (-10);
	}
	int offset = 0;
	pool_container_used = mem + offset;
	offset += sizeof(*pool_container_used);
	
	pool_container_freed = mem + offset;
	offset += sizeof(*pool_container_freed);
	
	pool_container = mem + offset;
	offset += sizeof(CONTAINER) * max_container_in_game;

	pool_thing_obj_used = mem + offset;
	offset += sizeof(*pool_thing_obj_used);

	pool_thing_obj_freed = mem + offset;
	offset += sizeof(*pool_thing_obj_freed);

	pool_thing_obj = mem + offset;
	offset += sizeof(THING_OBJ) * max_thing_obj_in_game;

	pool_role_used = mem + offset;
	offset += sizeof(*pool_role_used);

	pool_role_freed = mem + offset;
	offset += sizeof(*pool_role_freed);

	pool_role = mem + offset;
	offset += sizeof(ROLE) * max_role_in_game;

	pool_scene_used = mem + offset;
	offset += sizeof(*pool_scene_used);

	pool_scene_freed = mem + offset;
	offset += sizeof(*pool_scene_freed);

	pool_scene = mem + offset;
	offset += sizeof(SCENE) * max_scene_in_game;		

	pool_instance_used = mem + offset;
	offset += sizeof(*pool_instance_used);

	pool_instance_freed = mem + offset;
	offset += sizeof(*pool_instance_freed);

	pool_instance = mem + offset;
	offset += sizeof(INSTANCE) * max_instance_in_game;

	return (0);
}

int init_all_objs(int resume)
{
	if (resume) {
		if (resume_thing_obj() != 0) {
			return (-1); 		
		}
		if (resume_container() != 0) {
			return (-2);		
		}
		if (resume_scene() != 0) {
			return (-4); 		
		}
		if (resume_instance() != 0) {
			return (-5); 		
		}
		if (resume_role() != 0) {
			return (-6); 		
		}
	} else {
		init_thing_obj();
		init_container();
		init_role();
		init_scene();
		init_instance();
	}
	return (0);
}

int max_conn_srv;
CLIENT_MAP *active_conn_srv;
CLIENT_MAP conn_server[MAX_CONN_SRV];
CLIENT_MAP *active_roledb_srv;
int max_roledb_srv;
CLIENT_MAP roledb_server[MAX_ROLEDB_SRV];
/*
int test_broad()
{
	char buf[sizeof(PROTO_HEAD_CONN_BROADCAST) + sizeof(uint16_t) * 10];
	PROTO_HEAD *real_head;
	PROTO_HEAD_CONN_BROADCAST *head;
	head = (PROTO_HEAD_CONN_BROADCAST *)buf;
	
	head->len = htons(sizeof(buf));
	head->msg_id = SERVER_PROTO_BROADCAST;
	head->num_fd = 10;

	real_head = (PROTO_HEAD *)((char *)&head->real_head + sizeof(uint16_t) * head->num_fd);
	
	real_head->len = htons(sizeof(PROTO_HEAD));
	real_head->msg_id = htons(0xffea);
	send(conn_server.fd, buf, sizeof(buf), 0);
	return (0);
}
*/
static int gamesrv_cb_conn_recv(evutil_socket_t fd)
{
	int i;
	int ret;

	for (i = 0; i < max_conn_srv; ++i) {
		if (conn_server[i].fd == fd) {
			active_conn_srv = &conn_server[i];
			break;
		}
	}
	
	ret = get_one_buf(fd, active_conn_srv);
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: connect closed from fd %u", __FUNCTION__, fd);
		return (-1);		
	}
	else if (ret > 0) {
		return (0);
	}

	for (;;) {
		decode_client_proto(active_conn_srv, i);		
		ret = remove_one_buf(active_conn_srv);
		assert(ret >= 0);
		if (ret == 0)
			break;
	}
	return (0);
}
static int gamesrv_cb_roledb_recv(evutil_socket_t fd)
{
	int i;
	int ret;

	for (i = 0; i < max_roledb_srv; ++i) {
		if (roledb_server[i].fd == fd) {
			active_roledb_srv = &roledb_server[i];
			break;
		}
	}
	
	ret = get_one_buf(fd, active_roledb_srv);
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: connect closed from fd %u", __FUNCTION__, fd);
		return (-1);		
	}
	else if (ret > 0) {
		return (0);
	}

	for (;;) {
		handle_roledb_msg(active_roledb_srv);
		ret = remove_one_buf(active_roledb_srv);
		assert(ret >= 0);
		if (ret == 0)
			break;
	}
	return (0);	
}

static struct event log_object_timer;
static log4c_category_t* obj_count_loger = NULL;
static void cb_log_object_timer(evutil_socket_t fd, short events, void *arg)
{
	static struct timeval t1 = {5, 0};
	int num_container, num_thing_obj, num_role, num_scene, num_instance;
	add_timer(t1, &log_object_timer);
	log_object_timer.ev_callback = cb_log_object_timer;
	num_container = get_container_pool_num();	
	num_thing_obj = get_thing_obj_pool_num();
	num_role = get_role_pool_num();
	num_scene = get_scene_pool_num();
	num_instance = get_instance_pool_num();
	log4c_category_log(obj_count_loger, LOG4C_PRIORITY_INFO, "game_srv count obj: container[%d] thing_obj[%d] role[%d] scene[%d] instance[%d]",
		num_container, num_thing_obj, num_role, num_scene, num_instance);
}

int main(int argc, char *argv[])
{
	int i;
	int ret;
//	CONTAINER *tmp;
	int resume = 0;
	struct sockaddr_in sin;
	int16_t port;


	FILE *file;
	char *line;
	char *ip;

	srandom(time(NULL));
	
	ret = log4c_init();
	if (ret != 0) {
		printf("log4c_init failed[%d]\n", ret);		
		return ret;
	}
	mycat = log4c_category_get("six13log.log.app");
	if (!mycat) {
		printf("log4c_category_get(\"six13log.log.app.application1\"); failed\n");
		return (0);
	}

	obj_count_loger = log4c_category_get("count_obj_cate");
	if (!obj_count_loger) {
		printf("log4c_category_get(\"six13log.log.app.application2\"); failed\n");
		return (0);
	}

	for (i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-d") == 0) {
			change_to_deamon();
			--argc;
			break;
		}
	}
	if (argc > 1)
		resume = 1;
	
	ret = load_all_config();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "load all config fail[%d]\n", ret);
		return (-1);
	}

	ret = init_gm_system();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "init gm system fail[%d]\n", ret);
		return (-1);		
	}

	sc_interface = load_game_script();
	if (!sc_interface) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "load script fail[%d]\n");		
		return (-1);
	}
	
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "game_srv run");

	role_rbdata_init_head();	

	file = fopen("../server_info.ini", "r");
	if (!file) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "open server_info.ini failed[%d]", errno);				
		return (-1);
	}
	line = get_first_key(file, "game_srv_key");
	key = strtoul(get_value(line), NULL, 0);
	if (key == 0 || key == ULONG_MAX) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "config file wrong, no game_srv_key");
		return (-1);
	}
	
	if (alloc_shared_mem(resume) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "alloc_shared_mem[%d] failed", resume);		
		return (-1);
	}

	ret = init_all_objs(resume);
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "init_all_objs[%d] failed[%d]", resume, ret);
		return (-1);		
	}

	ret = create_all_static_scene();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "creeate_all_static_scene[%d] failed[%d]", resume, ret);
		return (-1);		
	}

//	test_fight();
//	return (0);
	
	ret = game_event_init();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "game_event_init failed[%d]", ret);		
		return (-1);
	}


	line = get_first_key(file, "conn_srv_ip");	
	for (i = 0; i < MAX_CONN_SRV && line != NULL; ++i) {
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		conn_server[i].cb_recv = gamesrv_cb_conn_recv;
		ip = get_value(line);
		if (!ip)
			break;
		line = get_next_key(file, "conn_srv_port");
		if (!line)
			break;
		port = atoi(get_value(line));
		if (port <= 0)
			break;
		line = get_next_key(file, "conn_srv_ip");
		sin.sin_port = htons(port);
		ret = evutil_inet_pton(AF_INET, ip, &sin.sin_addr);
		if (ret != 1) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "evutil_inet_pton failed[%d], ip[%s] port[%d]", ret, ip, port);
			return (-1);		
		}
		ret = game_add_connect_event((struct sockaddr *)&sin, sizeof(sin), &conn_server[i]);
		if (ret < 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "game_add_connect_event to conn server failed[%d], ip[%s] port[%d]", ret, ip, port);
			return (-1);		
		}
		conn_server[i].fd = ret;
	}
	if (i == 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "config file wrong, no conn_srv");		
		return (-1);				
	}
	max_conn_srv = i;

	line = get_first_key(file, "db_srv_ip");	
	for (i = 0; i < MAX_ROLEDB_SRV && line != NULL; ++i) {
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		roledb_server[i].cb_recv = gamesrv_cb_roledb_recv;		
		ip = get_value(line);
		if (!ip)
			break;
		line = get_next_key(file, "db_srv_port");
		if (!line)
			break;
		port = atoi(get_value(line));
		if (port <= 0)
			break;
		line = get_next_key(file, "db_srv_ip");
		sin.sin_port = htons(port);
		ret = evutil_inet_pton(AF_INET, ip, &sin.sin_addr);
		if (ret != 1) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "evutil_inet_pton failed[%d], ip[%s] port[%d]", ret, ip, port);
			return (-1);		
		}
		ret = game_add_connect_event((struct sockaddr *)&sin, sizeof(sin), &roledb_server[i]);
		if (ret < 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "game_add_connect_event to conn server failed[%d], ip[%s] port[%d]", ret, ip, port);
			return (-1);		
		}
		roledb_server[i].fd = ret;
	}
	if (i == 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "config file wrong, no roldb_srv");		
		return (-1);				
	}
	max_roledb_srv = i;	
	active_roledb_srv = &roledb_server[0];	
	fclose(file);

	struct timeval t1 = {5, 0};	
	add_timer(t1, &log_object_timer);
	log_object_timer.ev_callback = cb_log_object_timer;

	
	ret = event_base_loop(base, 0);
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "event_base_loop stoped[%d]", ret);		

/*	
	tmp = create_container();	
	tmp = create_container();
	tmp = create_container();
	destory_container(tmp);	
	tmp = create_container();
	destory_container(tmp);
	tmp = create_container();
*/

/*	
	GAME_CIRCLEQ_FOREACH(tmp, pool_container_freed, base.list_free) {
		printf("tmp[idx] = %d:%d\n", tmp->base.list_free.idx, tmp->base.list_used.idx);
	}


	printf("\n ================== \n");
	GAME_CIRCLEQ_FOREACH(tmp, pool_container_used, base.list_used) {
		printf("tmp[idx] = %d:%d\n", tmp->base.list_free.idx, tmp->base.list_used.idx);
	}
*/	
	return (0);
}
