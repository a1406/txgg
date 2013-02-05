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
#include "db_net.h"

extern log4c_category_t* mycat;

int create_role_handle(PROTO_HEAD *head, uint16_t fd, int len)
{
	int i;
	uint64_t effect = 0;
	CREATE_ROLE_REQUEST *req = (CREATE_ROLE_REQUEST *)&head->data;
	CREATE_ROLE_RESPONSE *resp;
	DbBlobRoleInfo blob_info;
	DbQuestProgressList quest;
	DbOneRole one_role;
	DbOneRole *one_role_point[1];
	DbBag bag;	

	uint32_t equip[MAX_EQUIP_PER_ROLE];
	uint32_t active_skill[MAX_ACTIVE_SKILL];
	
	uint8_t *pack_buf;
	size_t size;
	char sendbuf[1024];
	char sql[256];
	char *p = sql;

	for (i = 0; i < MAX_EQUIP_PER_ROLE; ++i)
		equip[i] = 0;
	for (i = 0; i < MAX_ACTIVE_SKILL; ++i)
		active_skill[i] = 0;	

	resp = (CREATE_ROLE_RESPONSE *)(&sendbuf[sizeof(PROTO_HEAD)]);
	head = (PROTO_HEAD *)(&sendbuf[0]);
	pack_buf = &(resp->info.blob_info[0]);

	db_quest_progress_list__init(&quest);
	db_bag__init(&bag);
	db_one_role__init(&one_role);
	db_blob_role_info__init(&blob_info);
	blob_info.one_role = one_role_point;
	blob_info.n_one_role = 1;
	one_role_point[0] = &one_role;
	blob_info.bag = &bag;
	bag.max_num = 30;
	one_role.name = req->name;
	one_role.level = 1;
	one_role.prof = req->profession;
	one_role.exp = 0;
	one_role.n_equip = MAX_EQUIP_PER_ROLE;
	one_role.equip = equip;
	one_role.n_active_skill = MAX_ACTIVE_SKILL;
	one_role.active_skill = active_skill;	
	blob_info.quest = &quest;
	size = db_blob_role_info__pack(&blob_info, pack_buf);

	p += sprintf(sql, "insert into DBRoleInfo set id = %lu, profession = %u, scene_id = 1, pos_x = 1000, pos_y = 500, name = \'",
		req->id, req->profession);
	p += escape_string(p, req->name, strlen(req->name));
	p += sprintf(p, "\', blob_info = \'");
	p += escape_string(p, (const char *)pack_buf, size);
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
	resp->info.money = 0;
	resp->info.gold = 0;	

	resp->info.blob_size = size;
done:
	head->len = htons(sizeof(PROTO_HEAD) + sizeof(*resp) + size);
	head->msg_id = htons(SERVER_PROTO_CREATE_ROLE_RESPONSE);
	resp->id = req->id;
	send_to_game_srv(fd, head);

	return (0);
}
