#ifndef _FUNCTION_TABLE_H__
#define _FUNCTION_TABLE_H__

#include "../../game-srv/instance.h"
#include "../../game-srv/role.h"
#include "../../game-srv/scene.h"


typedef int (*func_instance)(handle_instance);
typedef int (*func_role)(handle_role);
typedef int (*func_scene)(handle_scene);
typedef int (*func_instance_role)(handle_instance, handle_role);

#endif

