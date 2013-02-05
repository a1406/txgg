#include <stdio.h>
#include <sys/queue.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include "log4c.h"
#include "mempool.h"
#include "game_event.h"

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

log4c_category_t* mycat = NULL;
uint8_t toclient_send_buf[TOCLIENT_SEND_BUF_SIZE];

POOL_DEF(container)
POOL_DEF(thing_obj)
POOL_DEF(role)
POOL_DEF(scene)
POOL_DEF(instance)

const static key_t key = 0x10001020;

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
		if (resume_role() != 0) {
			return (-3); 		
		}
		if (resume_scene() != 0) {
			return (-4); 		
		}
		if (resume_instance() != 0) {
			return (-5); 		
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

CLIENT_MAP conn_server;
CLIENT_MAP roledb_server;
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
	int ret;
	ret = get_one_buf(fd, &conn_server);
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: connect closed from fd %u", __FUNCTION__, fd);
		return (-1);		
	}
	else if (ret > 0) {
		return (0);
	}

	for (;;) {
		decode_client_proto(&conn_server);		
		ret = remove_one_buf(&conn_server);
		assert(ret >= 0);
		if (ret == 0)
			break;
	}
	return (0);
}
static int gamesrv_cb_roledb_recv(evutil_socket_t fd)
{
	int ret;
	ret = get_one_buf(fd, &roledb_server);
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
			"%s: connect closed from fd %u", __FUNCTION__, fd);
		return (-1);		
	}
	else if (ret > 0) {
		return (0);
	}

	for (;;) {
		handle_roledb_msg(&roledb_server);
		ret = remove_one_buf(&roledb_server);
		assert(ret >= 0);
		if (ret == 0)
			break;
	}
	return (0);	
}

int main(int argc, char *argv[])
{
	int i;
	int ret;
//	CONTAINER *tmp;
	int resume = 0;

	conn_server.cb_recv = gamesrv_cb_conn_recv;
	roledb_server.cb_recv = gamesrv_cb_roledb_recv;
	
	ret = log4c_init();
	if (ret != 0) {
		printf("log4c_init failed[%d]\n", ret);		
		return ret;
	}
	mycat = log4c_category_get("six13log.log.app.application1");
	if (!mycat) {
		printf("log4c_category_get(\"six13log.log.app.application1\"); failed\n");
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
	
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "game_srv run");

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

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(BASE_SERVER_PORT + CONN_SRV_SERVER_DELTA);
	ret = evutil_inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);
	if (ret != 1) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "evutil_inet_pton failed[%d]", ret);		
		return (-1);		
	}

	ret = game_add_connect_event((struct sockaddr *)&sin, sizeof(sin), &conn_server);
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "game_add_connect_event to conn server failed[%d]", ret);		
//		return (-1);		
	}
	conn_server.fd = ret;

//	test_broad();
	
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(BASE_SERVER_PORT + DB_SRV_SERVER_DELTA);
	ret = evutil_inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);
	if (ret != 1) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "evutil_inet_pton failed[%d]", ret);		
		return (-1);		
	}
	ret = game_add_connect_event((struct sockaddr *)&sin, sizeof(sin), &roledb_server);
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "game_add_connect_event to roledb server failed[%d]", ret);		
		return (-1);		
	}
	roledb_server.fd = ret;

//	test_send_fetch_role(roledb_server.fd);
	
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
