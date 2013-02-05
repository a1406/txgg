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

#include <time.h>
#include <sys/time.h>
#include <signal.h>

#include "log4c.h"
#include "game_event.h"
#include "client_map.h"
#include "server_proto.h"
#include "game_net.h"
#include "cs.pb-c.h"
#include "login.h"
#include "deamon.h"
#include "define.h"
#include "oper_config.h"

uint16_t num_client_map;
testuser **usergroup;
log4c_category_t* mycat = NULL;
testuser userone;

//using for store test function id
int tslist[ 100 ];
int Max_Test_steps;


