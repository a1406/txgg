#ifndef _GAMESRV_SCENE_H__
#define _GAMESRV_SCENE_H__
#include "idx.h"
#include "role.h"

typedef struct scene_struct
{
	OBJ_BASE(scene_struct) base;
	uint16_t id;
	uint16_t num_roles;
	IDX roles[MAX_ROLE_PER_SCENE];
} SCENE, *handle_scene;

void init_scene_data(SCENE *scene);
int resume_scene_data(SCENE *scene);

int add_role_to_scene(SCENE *scene, ROLE *role);
int del_role_from_scene(SCENE *scene, ROLE *role);

SCENE *get_scene_by_id(uint16_t id);

int broadcast_to_scene(SCENE *scene, PROTO_HEAD_CONN_BROADCAST *head);
int broadcast_to_scene_except(SCENE *scene, uint16_t except_fd, PROTO_HEAD_CONN_BROADCAST *head);

int broadcast_move_notify(ROLE *role);
#endif
