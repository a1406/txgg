#ifndef _DB_NET_H__
#define _DB_NET_H__

int send_to_game_srv(int fd, PROTO_HEAD *head);
int send_all_cached_gamesrv_buf(int fd);

#endif
