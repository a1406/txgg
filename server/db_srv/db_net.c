#include <string.h>
#include <errno.h>
#include "server_proto.h"
#include "game_net.h"


#define MAX_CACHED_BUF_SIZE (1024 * 1024 * 10)
static int cached_len;
static char cached_buf[MAX_CACHED_BUF_SIZE];

static int cache_to_gamesrv_buf(PROTO_HEAD *head)
{
	int need_len = htons(head->len);
	
	if (need_len > MAX_CACHED_BUF_SIZE - cached_len)
		return (-1);
	memcpy(&cached_buf[cached_len], head, need_len);
	cached_len += need_len;
	return (0);
}

int send_all_cached_gamesrv_buf(int fd)
{
	PROTO_HEAD *head;
	int i;
	int len;

	if (cached_len <= 0)
		return (0);
	
	for (i = 0; i < cached_len; i += len) {
		head = (PROTO_HEAD *)&cached_buf[i];
		len = htons(head->len);
		if (send_one_msg(fd, head, 0) != len) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: send to gameserver failed err[%d]", __FUNCTION__, errno);
			cached_len = 0;
			return (-1);
		}
	}
	cached_len = 0;
	return (0);
}

int send_to_game_srv(int fd, PROTO_HEAD *head)
{
	if (send_one_msg(fd, head, 0) != htons(head->len)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
			"%s: failed, cache it", __FUNCTION__);
		if (cache_to_gamesrv_buf(head) != 0) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
				"%s: cache_to_gamesrv_buf failed", __FUNCTION__);			
		}
		return (-1);
	}
	return (0);
}
