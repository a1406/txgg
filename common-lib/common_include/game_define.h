#ifndef _GAME_DEFINE_H__
#define _GAME_DEFINE_H__

#define MAX_ROLE_NAME_LEN  (32)
#define MAX_ACTIVE_SKILL   (3)
#define MAX_SKILL_PER_ROLE (32)

#define MAX_SCENE_PER_INSTANCE (5)
#define MAX_ROLE_PER_INSTANCE (5)

#define MAX_THING_OBJ_PER_CONTAINER (128)

#define MAX_ROLE_PER_SCENE (200)

#define BASE_SERVER_PORT (6686)       //基础端口
#define CONN_SRV_CLIENT_DELTA (0)    //连接服务器开放给前台的端口和基础端口的差值
#define CONN_SRV_SERVER_DELTA (1)    //连接服务器开放给前台的端口和基础端口的差值
#define DB_SRV_SERVER_DELTA (2)      //DB服务器开放给前台的端口和基础端口的差值

#endif
