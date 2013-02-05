#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "mysql_module.h"
#include "log4c.h"
#include "server_proto.h"
#include "game_net.h"
#include "special_proto.h"
#include "cs.pb-c.h"
#include "db_net.h"

extern log4c_category_t* mycat;
char send_buf[1024];

int store_role_handle(PROTO_HEAD *head, uint16_t fd, int len)
{
	char *p;
	uint64_t effect = 0;	
	char sql[4096];
	STORE_ROLE_DATA_RESPONSE *resp;	
	char sendbuf[sizeof(PROTO_HEAD) + sizeof(*resp)];
	STORE_ROLE_DATA_REQUEST *request = (STORE_ROLE_DATA_REQUEST *)head->data;

	resp = (STORE_ROLE_DATA_RESPONSE *)(&sendbuf[sizeof(PROTO_HEAD)]);
	head = (PROTO_HEAD *)(&sendbuf[0]);

	p = sql;	
	p += sprintf(sql, "update DBRoleInfo set level = %d, scene_id = %d, pos_x = %d, pos_y = %d, money = %u, gold = %u, blob_info = \'",
		request->info.level, request->info.scene_id, request->info.pos.pos_x, request->info.pos.pos_y, request->info.money, request->info.gold);
	p += escape_string(p, (char *)&request->info.blob_info[0], request->info.blob_size);
	sprintf(p, "\' where id = %lu", request->id);
	
//	sprintf(sql, "update DBRoleInfo set level = %d, scene_id = %d, pos_x = %d, pos_y = %d, bag = where id = %lu",
//		request->info.level, request->info.scene_id, request->info.pos.pos_x, request->info.pos.pos_y, request->id);
	query(sql, 1, &effect);
	if (effect != 1) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s: can not store role %lu\n",
			__FUNCTION__, request->id);
		resp->result = -1;
		goto done;
	}
	resp->result = 0;
done:
	resp->id = request->id;
	head->len = htons(sizeof(sendbuf));
	head->msg_id = htons(SERVER_PROTO_STORE_ROLE_RESPONSE);
	send_to_game_srv(fd, head);
//	send(fd, sendbuf, sizeof(sendbuf), 0);	
	return (0);
}
