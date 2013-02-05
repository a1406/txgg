#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>

#include "cs.pb-c.h"
#include "server_proto.h"

static char sendbuf[1024 * 100];
static PROTO_HEAD *head = (PROTO_HEAD *)&sendbuf[0];

int send_move_request(uint16_t fd, uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y)
{
	size_t size;
	MoveReq req;

	move_req__init(&req);
	req.move_start_x = start_x;
	req.move_start_y = start_y;
	req.move_end_x = end_x;
	req.move_end_y = end_y;

	size = move_req__pack(&req, (uint8_t *)&(sendbuf[sizeof(*head)])) + sizeof(*head);
	head->msg_id = htons(CS__MESSAGE__ID__MOVE_REQUEST);
	head->len = htons(size);
	send(fd, sendbuf, size, 0);	
	return (0);
}

int send_login_request(uint16_t fd, uint32_t id)
{
	size_t size;
	LoginReq req;

	login_req__init(&req);
	req.role_id = id;
	
	size = login_req__pack(&req, (uint8_t *)&(sendbuf[sizeof(*head)])) + sizeof(*head);

	head->msg_id = htons(CS__MESSAGE__ID__LOGIN_REQUEST);
	head->len = htons(size);
	send(fd, sendbuf, size, 0);
	return 0;
}

int send_createrole_request(uint16_t fd, uint32_t id)
{
	size_t size;
	CreateroleReq req;
	char name[64];

	createrole_req__init(&req);
	req.role_id = id;
	req.profession = 2;
	sprintf(name, "testrole_%u_%u", id, fd);
	req.name = &name[0];
	
	size = createrole_req__pack(&req, (uint8_t *)&(sendbuf[sizeof(*head)])) + sizeof(*head);

	head->msg_id = htons(CS__MESSAGE__ID__CREATEROLE_REQUEST);
	head->len = htons(size);
	send(fd, sendbuf, size, 0);
	
	return 0;
}


int send_transport_request()
{
	return (0);
}
