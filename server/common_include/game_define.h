#ifndef _GAME_DEFINE_H__
#define _GAME_DEFINE_H__

#define MAX_CONN_SRV  (8)
#define MAX_ROLEDB_SRV  (16)

#define MAX_EQUIP_PER_ROLE (8)
//伙伴
#define MAX_FRIEND_PER_ROLE (32)
//游戏好友
#define MAX_GAME_FRIEND_PER_ROLE (256)  

#define MAX_SLAVE_QUEST    (1024)
#define MAX_QUEST_PER_ROLE (64)
#define MAX_BUFF_PER_ROLE (16)

#define MAX_ROLE_NAME_LEN  (32)
#define MAX_ACTIVE_SKILL   (3)
#define MAX_SKILL_PER_ROLE (128)
#define MAX_SHORTCUT_PER_ROLE (5)

#define MAX_SCENE_PER_INSTANCE (5)
#define MAX_ROLE_PER_INSTANCE (5)
#define MAX_NODE_PER_INSTANCE (512)

#define MAX_THING_OBJ_PER_CONTAINER (128)

#define MAX_ROLE_PER_SCENE (200)

#define MAX_THINGLIST_NUM (64)	

#define MAX_DROP_THING_PER_DATA  (64)
#define MAX_DROP_RATE  (100000)

/*
#define BASE_SERVER_PORT (6686)       //基础端口
#define CONN_SRV_CLIENT_DELTA (0)    //连接服务器开放给前台的端口和基础端口的差值
#define CONN_SRV_SERVER_DELTA (1)    //连接服务器开放给前台的端口和基础端口的差值
#define DB_SRV_SERVER_DELTA (2)      //DB服务器开放给前台的端口和基础端口的差值
*/
#endif
