#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "cs.pb-c.h"
#include "server_proto.h"
#include "log4c.h"

extern log4c_category_t* mycat;

static char sendbuf[1024 * 100];
static PROTO_HEAD *head = (PROTO_HEAD *)&sendbuf[0];

int send_chat_request( uint16_t fd, char * chatstring )
{
    size_t size;
    ChatRequest req;

    chat_request__init(&req);
    req.channel = 0;
    req.target_role_id = 0;
    req.target_area_id = 0;
    req.msg = chatstring;

    size = chat_request__pack(&req, (uint8_t *)&(sendbuf[sizeof(*head)])) + sizeof(*head);
    head->msg_id = htons(CS__MESSAGE__ID__CHAT_REQUEST);
	head->len = htons(size);
	send(fd, sendbuf, size, 0);	
	return (0);
}

int send_enter_instance_request(uint16_t fd, uint32_t instance_id)
{
	size_t size;
	EnterInstanceReq req;

	enter_instance_req__init(&req);
	req.instance_id = instance_id;

	size = enter_instance_req__pack(&req, (uint8_t *)&(sendbuf[sizeof(*head)])) + sizeof(*head);
	head->msg_id = htons(CS__MESSAGE__ID__ENTER_INSTANCE_REQUEST);
	head->len = htons(size);
	send(fd, sendbuf, size, 0);	
	return (0);
}

int send_leave_instance_request(uint16_t fd, uint32_t instance_id)
{
	size_t size;

	size = sizeof(*head);
	head->msg_id = htons(CS__MESSAGE__ID__LEAVE_INSTANCE_REQUEST);
	head->len = htons(size);
	send(fd, sendbuf, size, 0);	
	return (0);
}

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

//	sleep(2);
 	createrole_req__init(&req);
	req.role_id = id;
	req.profession = ( rand(  ) % 4) + 1;

	sprintf(name, "testrole_%u_%u_%u", id, fd,req.profession);
        log4c_category_log(mycat, LOG4C_PRIORITY_INFO,"%s: testrole_%u_%u profession %u\n",
                           __FUNCTION__,  id, fd,req.profession);

	req.name = &name[0];
	
	size = createrole_req__pack(&req, (uint8_t *)&(sendbuf[sizeof(*head)])) + sizeof(*head);

	head->msg_id = htons(CS__MESSAGE__ID__CREATEROLE_REQUEST);
	head->len = htons(size);
	int ret = send(fd, sendbuf, size, 0);
	printf("ret = %d, size = %d, err = %d\n", ret, (int )size, errno);
	
	return 0;
}


int send_transport_request()
{
	return (0);
}
