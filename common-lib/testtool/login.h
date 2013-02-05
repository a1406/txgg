#ifndef _TESTTOOL_LOGIN_H__
#define _TESTTOOL_LOGIN_H__

int decode_server_proto(CLIENT_MAP *server);
int send_login_request(uint16_t fd, uint32_t id);
int send_createrole_request(uint16_t fd, uint32_t id);
int send_move_request(uint16_t fd, uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y);
#endif
