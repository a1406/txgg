#include <assert.h>
#include <search.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>

#include "log4c.h"
#include "game_event.h"
#include "client_map.h"
#include "game_net.h"

log4c_category_t* mycat = NULL;

int conn_cb_client_listen_pre()
{
		//todo check client_map size
	return (0);
}

int conn_cb_client_listen_after(evutil_socket_t fd)
{
	return (0);
}

int conn_cb_client_recv(evutil_socket_t fd)
{
//	static char *sendbuf = "<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>";
	static char *sendbuf = "<allow-access-from domain=\"*\" /><cross-domain-policy><allow-access-from domain=\"*\" /></cross-domain-policy>\\0";
	char buf[1024];
	if (recv(fd, buf, 1024, 0) <= 0)
		return (-1);
	send(fd, sendbuf, strlen(sendbuf) + 1, 0);
	return (0);
}

static LISTEN_CALLBACKS callback_client;
int main(int argc, char **argv)
{
	int ret = 0;
	
	ret = log4c_init();
	if (ret != 0) {
		printf("log4c_init failed[%d]\n", ret);
		return (ret);
	}
	mycat = log4c_category_get("six13log.log.app.application1");
	if (!mycat) {
		printf("log4c_category_get(\"six13log.log.app.application1\"); failed\n");
		return (0);
	}
    log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "flashsrvd run");

	ret = game_event_init();
	if (ret != 0)
		goto done;

	callback_client.cb_recv = conn_cb_client_recv;
	callback_client.cb_listen_pre = conn_cb_client_listen_pre;
	callback_client.cb_listen_after = conn_cb_client_listen_after;	
	
	ret = game_add_listen_event(8080, &callback_client);
	if (ret != 0)
		goto done;

	
	ret = event_base_loop(base, 0);
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "event_base_loop stoped[%d]", ret);	

	struct timeval tv;
	event_base_gettimeofday_cached(base, &tv);
done:
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "flashsrvd stoped[%d]", ret);
	return (ret);
}
