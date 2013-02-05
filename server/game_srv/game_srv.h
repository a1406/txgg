#ifndef _GAME_GAME_SRV_H__
#define _GAME_GAME_SRV_H__

#include <stdio.h>
#include <sys/queue.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <sys/ipc.h>
#include <sys/shm.h>

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
#include "client_map.h"
#include "script_define.h"
#include "script.h"

#define  max_container_in_game    (4096 * 2)
#define  max_thing_obj_in_game   (10240)
#define  max_role_in_game   (4096)
#define  max_scene_in_game   (4096)
#define  max_instance_in_game   (4096)

#define TOCLIENT_SEND_BUF_SIZE (1024 * 1024)
extern log4c_category_t* mycat;
extern uint8_t toclient_send_buf[TOCLIENT_SEND_BUF_SIZE];

extern CLIENT_MAP *active_conn_srv;
extern CLIENT_MAP conn_server[MAX_CONN_SRV];
extern CLIENT_MAP *active_roledb_srv;
extern CLIENT_MAP roledb_server[MAX_ROLEDB_SRV];
extern int max_conn_srv;
extern int max_roledb_srv;

POOL_DEC(thing_obj)
POOL_DEC(container)
POOL_DEC(role)
POOL_DEC(scene)
POOL_DEC(instance)

#endif


