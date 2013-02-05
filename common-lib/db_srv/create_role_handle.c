#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include "mysql_module.h"
#include "log4c.h"
#include "server_proto.h"
#include "game_net.h"
#include "special_proto.h"

extern log4c_category_t* mycat;

int create_role_handle(PROTO_HEAD *head, uint16_t fd, int len)
{
	uint64_t effect = 0;
	CREATE_ROLE_REQUEST *req = (CREATE_ROLE_REQUEST *)&head->data;
	CREATE_ROLE_RESPONSE *resp;
	
	char sendbuf[sizeof(PROTO_HEAD) + sizeof(*resp)];
	char sql[256];
	char *p = sql;

	resp = (CREATE_ROLE_RESPONSE *)(&sendbuf[sizeof(PROTO_HEAD)]);
	head = (PROTO_HEAD *)(&sendbuf[0]);
	
	p += sprintf(sql, "insert into DBRoleInfo set id = %lu, profession = %u, scene_id = 1, pos_x = 1000, pos_y = 500, name = \'",
		req->id, req->profession);
	p += escape_string(p, req->name, strlen(req->name));
	*p++ = '\'';
	*p++ = '\0';
	query(sql, 1, &effect);	
	if (effect != 1) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s: execute query for %u %s failed\n",
			__FUNCTION__, req->id, req->name);
		resp->result = -1;
		goto done;
	}
	resp->result = 0;
	memcpy(resp->info.name, req->name, sizeof(resp->info.name));	
	resp->info.profession = req->profession;
	resp->info.level = 1;
	resp->info.scene_id = 1;
	resp->info.pos.pos_x = 1000;
	resp->info.pos.pos_y = 500;
done:
	head->len = htons(sizeof(sendbuf));
	head->msg_id = htons(SERVER_PROTO_CREATE_ROLE_RESPONSE);
	resp->id = req->id;
	send_one_msg(fd, head);
	return (0);
}
