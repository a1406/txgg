#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "game_net.h"
#include "log4c.h"

extern log4c_category_t* mycat;
#ifdef CALC_NET_MSG
uint32_t send_buf_times[CS__MESSAGE__ID__MAX_MSG_ID];
uint32_t recv_buf_times[CS__MESSAGE__ID__MAX_MSG_ID];
uint32_t send_buf_size[CS__MESSAGE__ID__MAX_MSG_ID];
uint32_t recv_buf_size[CS__MESSAGE__ID__MAX_MSG_ID];
#endif

int send_one_msg(evutil_socket_t fd, PROTO_HEAD *head, uint8_t force)
{
	int ret;
	char *p = (char *)head;
	int len = htons(head->len);
	for (;;) {
		ret = send(fd, p, len, 0);
		assert(ret <= len);
		if (ret == len)
			goto done;
		if (ret < 0) {
			if (errno != EAGAIN || force)
				goto fail;
				//ignore EINPROGRESS
			usleep(100);			
		} else if (ret < len) {
			len -= ret;
			p += ret;
			usleep(10);
		}
	}
done:	
	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG, "%s %d: send msg[%d] len[%d], ret [%d]",
		__FUNCTION__, fd, htons(head->msg_id), htons(head->len), ret);
#ifdef CALC_NET_MSG
	uint16_t id = htons(head->msg_id);
	if (id < CS__MESSAGE__ID__MAX_MSG_ID) {
		send_buf_size[id] += len;
		++send_buf_times[id];
	}
#endif	
	
	return (htons(head->len));
fail:
	log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: msg[%d] len[%d] ret[%d] errno[%d]",
		__FUNCTION__, fd, htons(head->msg_id), htons(head->len), ret, errno);			
	return ret;
}

//返回0表示接收完毕，返回大于0表示没接收完毕。返回小于零表示断开
int get_one_buf(evutil_socket_t fd, CLIENT_MAP *client)
{
	assert(client);
	int ret = -1;
	PROTO_HEAD *head;
	int len;
	
	ret = recv(client->fd, CLIENT_MAP_BUF_TAIL(client), CLIENT_MAP_BUF_LEAVE(client), 0);
	if (ret <= 0)     //连接断开
		return (-1);

	client->pos_end += ret;
	len = CLIENT_MAP_BUF_SIZE(client);	

	if (len < sizeof(PROTO_HEAD))  //没有够一个包头
		return (1);

	head = (PROTO_HEAD *)CLIENT_MAP_BUF_HEAD(client);
	if (len >= htons(head->len)) { //读完了
		log4c_category_log(mycat, LOG4C_PRIORITY_TRACE, "%s %d: get msg[%d] len[%d], max_len [%d]",
			__FUNCTION__, client->fd, htons(head->msg_id), htons(head->len), len);
#ifdef CALC_NET_MSG
		uint16_t id = htons(head->msg_id);
		if (id < CS__MESSAGE__ID__MAX_MSG_ID) {
			++recv_buf_times[id];			
			recv_buf_size[id] += htons(head->len);
		}
#endif	
		return (0);
	}

	log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: len not enough, msg[%d] len[%d], max_len [%d]",
		__FUNCTION__, client->fd, htons(head->msg_id), htons(head->len), len);	
	return (1);    //没有读完
}

//返回0表示正常，返回大于0表示还有包没有处理完, 不会小于0
int remove_one_buf(CLIENT_MAP *client)
{
	PROTO_HEAD *head;
	int buf_len;
	int msg_id;
	int len = CLIENT_MAP_BUF_SIZE(client);
	assert(len >= sizeof(PROTO_HEAD));

	head = (PROTO_HEAD *)CLIENT_MAP_BUF_HEAD(client);
	buf_len = htons(head->len);
	msg_id = htons(head->msg_id);
	assert(len >= buf_len);

	memmove(CLIENT_MAP_BUF_HEAD(client),
		CLIENT_MAP_BUF_HEAD(client) + buf_len,
		len - buf_len);
	client->pos_end -= buf_len;
	head = (PROTO_HEAD *)CLIENT_MAP_BUF_HEAD(client);
	
	len = CLIENT_MAP_BUF_SIZE(client);

	log4c_category_log(mycat, LOG4C_PRIORITY_TRACE, "%s %d: msg[%d] len[%d], reset_len [%d]",
		__FUNCTION__, client->fd, msg_id, buf_len, len);		
	
	if (len >= sizeof(PROTO_HEAD)
		&& len >= htons(head->len))
		return (1);
	return (0);
}
