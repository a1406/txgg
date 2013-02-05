#ifndef _TESTTOOL_LOGIN_H__
#define _TESTTOOL_LOGIN_H__

int decode_server_proto(CLIENT_MAP *server, uint32_t pos_arry);
int send_login_request(uint16_t fd, uint32_t id);
int send_createrole_request(uint16_t fd, uint32_t id);
int send_move_request(uint16_t fd, uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y);
int send_enter_instance_request(uint16_t fd, uint32_t instance_id);
int send_leave_instance_request(uint16_t fd, uint32_t instance_id);
int send_chat_request( uint16_t fd, char * charstring );

#endif
