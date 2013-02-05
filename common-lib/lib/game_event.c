#include "game_event.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <netinet/tcp.h>

#include "util-internal.h"
#include "client_map.h"

struct event_base *base = NULL;

static void libevent_log(int severity, const char *msg)
{
	log4c_category_log(mycat, severity, msg);
}

static void cb_signal(evutil_socket_t fd, short events, void *arg)
{
	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG, "%s: fd = %d, events = %d, arg = %p", __FUNCTION__, fd, events, arg);
}

static void cb_timer(evutil_socket_t fd, short events, void *arg)
{
	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG, "%s: fd = %d, events = %d, arg = %p", __FUNCTION__, fd, events, arg);
	if (arg)
		event_free(arg);
}

void remove_listen_callback_event(CLIENT_MAP *client_map)
{
	if (!client_map)
		return;
	if (client_map->fd == 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: fd is already closed", __FUNCTION__);
		return;
	}
	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG, "%s: close connect from fd %u", __FUNCTION__, client_map->fd);
	
	event_del(&client_map->event_recv);
	evutil_closesocket(client_map->fd);
	if (client_map->cb_release)
		client_map->cb_release(client_map);	
}

void cb_recv(evutil_socket_t fd, short events, void *arg)
{
	assert(arg);
	CLIENT_MAP *client_map = (CLIENT_MAP *)arg;

	if (client_map->cb_recv(fd) >= 0)
		return;

//	if (unlikely(events == EV_WRITE))
//		return;
	
	remove_listen_callback_event(client_map);	
}

void cb_listen(evutil_socket_t fd, short events, void *arg)
{
	assert(arg);
	CLIENT_MAP *client_map = NULL;
	int new_fd;
	LISTEN_CALLBACKS *callback = (LISTEN_CALLBACKS *)arg;
	if (callback->cb_listen_pre && callback->cb_listen_pre() < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s: connect pre refused", __FUNCTION__);		
		goto fail;
	}
	
	log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG, "%s: fd = %d, events = %d, arg = %p", __FUNCTION__, fd, events, arg);
	struct sockaddr_storage ss;
	size_t socklen = sizeof(ss);
	new_fd = accept(fd, (struct sockaddr*)&ss, (socklen_t *)&socklen);
	if (new_fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: accept failed[%d]", __FUNCTION__, __LINE__, errno);
		goto fail;
	}
	game_set_socket_opt(new_fd);	

	client_map = callback->cb_get_client_map(new_fd);
	if (!client_map) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: cb_get_client_map with fd[%u] failed", __FUNCTION__, __LINE__, new_fd);		
		goto fail;
	}
	
	if (0 != event_assign(&client_map->event_recv, base, new_fd, EV_READ | EV_PERSIST, cb_recv, client_map)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: event_assign failed[%d]", __FUNCTION__, __LINE__, errno);
		goto fail;
	}

	if (event_add(&client_map->event_recv, NULL) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: event_assign failed[%d]", __FUNCTION__, __LINE__, errno);
		goto fail;			
	}

	if (callback->cb_listen_after && callback->cb_listen_after(new_fd) < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s: connect after refused", __FUNCTION__);
		goto fail;
	}
	return;
fail:
	if (client_map) {
		remove_listen_callback_event(client_map);
	}
	if (new_fd > 0)
		evutil_closesocket(new_fd);			
}

int game_setnagleoff(int fd)
{
    /* Disable the Nagle (TCP No Delay) algorithm */ 
    int flag = 1; 

    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
    return 0;
}

int game_set_socket_opt(int fd)
{
	evutil_make_socket_nonblocking(fd);
	game_setnagleoff(fd);	
	
	int on = 1;
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&on, sizeof(on));
	return (0);
}

int create_new_socket(int set_opt)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd <= 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: socket failed[%d]", __FUNCTION__, __LINE__, errno);		
		return fd;
	}

	if (likely(set_opt))
		game_set_socket_opt(fd);
	return fd;
}

int add_timer(struct timeval t, struct event *event_timer)
{
	if (!event_timer) {
		event_timer = evtimer_new(base, cb_timer, NULL);
		if (!event_timer) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: evtimer_new failed[%d]", __FUNCTION__, __LINE__, errno);					
			return (-1);
		}
		event_timer->ev_arg = event_timer;
	} else {
		evtimer_assign(event_timer, base, event_timer->ev_callback, NULL);
	}

	return evtimer_add(event_timer, &t);
}

int add_signal(int signum, struct event *event)
{
	if (!event) {
		event = evsignal_new(base, signum, cb_signal, NULL);
		if (!event) {
			log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: evsignal_new failed[%d]", __FUNCTION__, __LINE__, errno);					
			return (-1);
		}
		event->ev_arg = event;
	} else {
		evsignal_assign(event, base, signum, event->ev_callback, NULL);
	}

	return evsignal_add(event, NULL);
}

int game_event_init()
{
	int ret = 0;
	struct event_config *config = NULL;
//	struct event *event_signal1 = NULL;
//	struct event *event_signal2 = NULL;	
	
	event_set_log_callback(libevent_log);

	config = event_config_new();
	if (!config)
	{
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: event_config_new failed[%d]", __FUNCTION__, __LINE__, errno);
		ret = -10;
		goto fail;
	}
	event_config_require_features(config, EV_FEATURE_ET | EV_FEATURE_O1 /*| EV_FEATURE_FDS*/);
	event_config_set_flag(config, EVENT_BASE_FLAG_NOLOCK | EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST);
	base = event_base_new_with_config(config);
	if (!base)
	{
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: event_base_new_with_config failed[%d]", __FUNCTION__, __LINE__, errno);
		ret = -20;
		goto fail;
	}
/*
	event_signal1 = evsignal_new(base, SIGUSR1, cb_signal, NULL);
	if (!event_signal1) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: evsignal_new failed[%d]", __FUNCTION__, __LINE__, errno);
		ret = -30;
		goto fail;
	}
	event_signal2 = evsignal_new(base, SIGUSR2, cb_signal, NULL);
	if (!event_signal2) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: evsignal_new failed[%d]", __FUNCTION__, __LINE__, errno);
		ret = -30;
		goto fail;
	}
	
//	struct event event_signal, event_timer;
//	evsignal_assign(&event_signal, base, SIGUSR1, cb_signal, NULL);
//	evtimer_assign(&event_timer, base, cb_timer, NULL);
	evsignal_add(event_signal1, NULL);
	evsignal_add(event_signal2, NULL);	
*/	
	return (0);
	
fail:
/*	
	if (event_signal1) {
		event_free(event_signal1);
		event_signal1 = NULL;
	}
	if (event_signal2) {
		event_free(event_signal2);
		event_signal2 = NULL;
	}
*/	
	if (config) {
		event_config_free(config);
		config = NULL;
	}
	if (base) {
		event_base_free(base);
		base = NULL;
	}
	return (ret);
}

int game_add_listen_event(uint16_t port, LISTEN_CALLBACKS *callback)
{
	struct event *event_accept = &callback->event_accept;
	int fd = 0;
	struct sockaddr_in addr;

	if (!callback || !callback->cb_get_client_map)
		return (-1);
	
	fd = create_new_socket(1);
	if (fd < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: create new socket failed[%d]", __FUNCTION__, errno);
		return (-2);		
	}
	evutil_make_listen_socket_reuseable(fd);	
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0);
	addr.sin_port = htons(port);	
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: bind failed[%d]", __FUNCTION__, errno);
		return (-10);
	}
	if (listen(fd, 128) != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: listen failed[%d]", __FUNCTION__, errno);
		return (-20);		
	}
	if (0 != event_assign(event_accept, base, fd, EV_READ|EV_PERSIST, cb_listen, (void *)callback)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: event_new failed[%d]", __FUNCTION__, errno);
		goto fail;
	}
	event_add(event_accept, NULL);
	return (0);
fail:
	if (fd > 0) {
		evutil_closesocket(fd);				
	}
	if (event_accept) {
		event_del(event_accept);				
	}
	return (-1);
}

int game_add_connect_event(struct sockaddr *sa, int socklen, CLIENT_MAP *client_map)
{
	int ret;
	struct event *event_conn = &client_map->event_recv;
	int fd = 0;
	
	fd = create_new_socket(0);
	if (0 != event_assign(event_conn, base, fd, EV_READ|EV_PERSIST, cb_recv, client_map)) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: event_new failed[%d]", __FUNCTION__, __LINE__, errno);				
		goto fail;
	}
	event_add(event_conn, NULL);
	
	ret = evutil_socket_connect(&fd, sa, socklen);
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s %d: evutil_socket_connect failed[%d]", __FUNCTION__, __LINE__, errno);		
		goto fail;
	}

	game_set_socket_opt(fd);
	
	return (fd);
fail:
	if (fd > 0) {
		evutil_closesocket(fd);		
	}
	if (event_conn) {
		event_del(event_conn);		
	}
	return (-1);	
}

