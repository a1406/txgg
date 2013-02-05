#include "cs.pb-c.h"
#include "../lib/client_map.h"
#include "../common_include/server_proto.h"

int decode_client_proto(CLIENT_MAP *conn_server)
{

	PROTO_HEAD_CONN *head = (PROTO_HEAD_CONN *)conn_server->buf;
	switch (head->msg_id)
	{
		case MSGID_LOGIN_REQUEST:
			login_req__unpack(NULL, head->len, (const uint8_t *)head->data);
			break;
		case MSGID_CREATEROLE_REQUEST:
			createrole_req__unpack(NULL, head->len, (const uint8_t *)head->data);
			break;
		default:
			break;
	}

	return (0);
}

int main()
{
	CLIENT_MAP a;

	decode_client_proto(&a);
	return (0);
}
