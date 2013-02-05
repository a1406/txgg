#ifndef _DBSRV_HANDLE_H__
#define _DBSRV_HANDLE_H__

int test_fetch_role();
int fetch_role_handle(PROTO_HEAD *head, uint16_t fd, int len);
int create_role_handle(PROTO_HEAD *head, uint16_t fd, int len);
int store_role_handle(PROTO_HEAD *head, uint16_t fd, int len);
#endif
