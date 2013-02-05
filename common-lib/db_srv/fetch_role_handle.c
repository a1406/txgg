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

extern log4c_category_t* mycat;
char send_buf[1024];

int test_fetch_role()
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	char sql[256];
	int len;
	char *p;
	uint64_t effect;
	
	len = sprintf(sql, "insert into DBRoleInfo set id = 8686, profession = 1, level = 1, name = ");
	p = sql + len;
	*p++ = '\'';
	p += escape_string(p, "jacktang", 8);
	*p++ = '\'';
	query(sql, 1, &effect);	

	sprintf(sql, "select id, name, profession, level, attribute, bag from DBRoleInfo where id = %lu", 8686l);
	res = query(sql, 1, NULL);
	row = fetch_row(res);

	row[0] = row[0];

	free_query(res);
	return (0);
}

int fetch_role_handle(PROTO_HEAD *head, uint16_t fd, int len)
{
//	int i;
	MYSQL_RES *res;
	MYSQL_ROW row;	
	char sql[256];
	FETCH_ROLE_DATA_RESPONSE *resp;	
	char sendbuf[sizeof(PROTO_HEAD) + sizeof(*resp)];
	FETCH_ROLE_DATA_REQUEST *request = (FETCH_ROLE_DATA_REQUEST *)head->data;
	

	resp = (FETCH_ROLE_DATA_RESPONSE *)(&sendbuf[sizeof(PROTO_HEAD)]);
	head = (PROTO_HEAD *)(&sendbuf[0]);

	resp->info.name[0] = '\0';
	
	if (len != sizeof(PROTO_HEAD) + sizeof(FETCH_ROLE_DATA_REQUEST)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s: len[%d] with msg[%u] server[?] fail\n",
			__FUNCTION__, head->len, head->msg_id);
		return (-1);
	}
	sprintf(sql, "select id, name, profession, level, scene_id, pos_x, pos_y from DBRoleInfo where id = %lu", request->id);
	res = query(sql, 1, NULL);
	if (!res) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s: can not fetch role %lu\n",
			__FUNCTION__, request->id);
		resp->result = -1;
		goto done;
	}
	row = fetch_row(res);
	if (!row) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s: can not fetch role %lu\n",
			__FUNCTION__, request->id);
		resp->result = -1;
		goto done;		
	}
	resp->result = 0;
	strcpy(resp->info.name, row[1]);
	resp->info.profession = atoi(row[2]);
	resp->info.level = atoi(row[3]);
	resp->info.scene_id = atoi(row[4]);
	resp->info.pos.pos_x = atoi(row[5]);
	resp->info.pos.pos_y = atoi(row[6]);				

//	resp->bag.max_thing_num = 10;
done:
	if (res)
		free_query(res);
	resp->id = request->id;
	head->len = htons(sizeof(sendbuf));
	head->msg_id = htons(SERVER_PROTO_FETCH_ROLE_RESPONSE);
	send_one_msg(fd, head);
//	send(fd, sendbuf, sizeof(sendbuf), 0);	
	return (0);
}
