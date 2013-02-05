#include "testtool.h"
#include <time.h>
#include <sys/time.h>
#include <signal.h>

static int servernum;
static int16_t conn_port;
static struct itimerval oldtv;
static int16_t  teststeps;

static int STATISTICS_TIMER = 30;
static int test_step_init ( void )
{
    Max_Test_steps = teststeps;
    tslist[1] = TID_CHAT_REQUEST;
    tslist[0] = TID_MOVE_REQUEST;
    tslist[2] = TID_ENTER_INSTANCE_REQUEST;
    tslist[3] = TID_LEAVE_INSTANCE_REQUEST;
    tslist[6] = TID_LOGIN_REQUEST;
    tslist[5] = TID_CHAT_REQUEST;

    return 0;
}

void execute_test_step(count)
{
    int test_step = usergroup[ count ]->testid;
    uint32_t  x,y,i,temp_y,temp_x;
    char p[ 100 ] = {0};
    
    if( test_step < Max_Test_steps )
    {
        log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
                           "%s: test_step %d start next step when max %d, count %d",
                           __FUNCTION__, test_step,Max_Test_steps,count);
        switch (tslist[test_step])
        {
        case TID_CHAT_REQUEST:            
                    for ( i = 0 ; i < Id_Count ; i++ )
                    {
                        sprintf(( char * )p, "Hi, i am testrole_%u_%u!", Id_Base + i,
                                usergroup[i]->client_maps.fd);
                        log4c_category_log(mycat, LOG4C_PRIORITY_TRACE,"Send chatstring %s!",
                                           p);
                        send_chat_request( usergroup[i]->client_maps.fd, p);
                    }

                    sleep( 1 );
            break;
        case TID_SLEEP:
            usergroup[count]->testid = ++test_step;
            log4c_category_log(mycat, LOG4C_PRIORITY_INFO,
                               "%s: test in sleep ",
                               __FUNCTION__);
            break;
            
        case TID_LOGIN_REQUEST:
            usergroup[ count ]->testid = ++test_step;
            send_login_request(usergroup[count]->client_maps.fd, usergroup[count]->userid);
            break;
            
        case TID_MOVE_REQUEST:
            if ( count == Id_Count -1 )
            {
                for( ;; )
                {
                    for ( i = 0 ; i < Id_Count ; i++ )
                    {
                        x = usergroup[ i ]->pos_x;
                        y = usergroup[ i ]->pos_y;
 
                        log4c_category_log(mycat, LOG4C_PRIORITY_TRACE,"start moving !");
                        send_move_request(usergroup[i]->client_maps.fd, x, y,
                                          temp_x = rand( ) % 2500 ,
                                          temp_y = rand( ) % 300 + 300);
                        log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
                                           "%s: start at pos_x[%d] pos_y[%d]!",
                                           __FUNCTION__,x,y);
                       
                        log4c_category_log(mycat, LOG4C_PRIORITY_DEBUG,
                                           "%s: move to pos_x[%d] pos_y[%d]!",
                                           __FUNCTION__,temp_x,temp_y);
                        usergroup[ i ]->pos_x = temp_x;
                        usergroup[ i ]->pos_y = temp_y;
 
                    }
                    sleep( 1 );
                }
                
                                    

            }
            usergroup[ count ]->testid = ++test_step;
            break;
        case TID_ENTER_INSTANCE_REQUEST:
            log4c_category_log(mycat, LOG4C_PRIORITY_TRACE,"start enter instance !");
            send_enter_instance_request( usergroup[count]->client_maps.fd,1 );
            usergroup[ count ]->testid = ++test_step;
            break;
        case TID_LEAVE_INSTANCE_REQUEST:
            send_leave_instance_request( usergroup[count]->client_maps.fd,1 );
            usergroup[ count ]->testid = ++test_step;
            break;
            
            break;            

        }
    }
    else
        log4c_category_log(mycat, LOG4C_PRIORITY_NOTICE,
                           "%s: test_step %d had reach or above max %d, count %d",
                           __FUNCTION__, test_step,Max_Test_steps,count);

}

void set_timer( int timer_value )
{
    struct itimerval itv;
    itv.it_interval.tv_sec = timer_value;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = timer_value;
    itv.it_value.tv_usec = 0;
    setitimer( ITIMER_REAL, &itv, &oldtv );
    
}

void signal_handler( int m )
{
    int i;
    int login = 0 ,in_instance = 0 ,in_scene = 0 ,init = 0;
    int failnum = 0;
    
    for ( i = 0; i < Id_Count ; i++ )
    {
        switch( usergroup[ i ]->userstate )
        {
        case USER_INIT:
            init++;
            break;
            
        case USER_LOGIN:
            login ++;
            break;
            
        case USER_IN_INSTANCE:
            login ++;
            in_instance ++;
            break;
            
        case USER_IN_SCENE:
            login++;
            in_scene ++;
            break;
            
        default:
            failnum++;
        }
    }
    printf( "====================statistics result========================== !\n");
    printf( "test user count is %d!\n",Id_Count );
    printf( "fail %d !\n",failnum );
    printf( "User login is %d !\n", login );
    printf( "User init  is %d !\n", init );
    printf( "User in Scene is %d!\n",in_scene );
    printf( "User in instance is %d!\n\n",in_instance  );
}

CLIENT_MAP *search_client_map_by_fd(uint16_t fd , int * pos_arry)
{
	int i;
	for (i = 0; i < Id_Count ; ++i) {
		if (usergroup[i]->client_maps.fd == fd)
            {
                * pos_arry = i;
                return ( CLIENT_MAP * ) &(usergroup[i]->client_maps);
            }
	}
	return NULL;
}

static int testtool_cb_conn_recv(evutil_socket_t fd)
{
	CLIENT_MAP *client;
	int ret;
    int count;
    
	client = search_client_map_by_fd(fd, &count);
	if (!client) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
                           "%s: can not find client map by fd %u", __FUNCTION__, fd);
		return (-1);				
	}
	
	ret = get_one_buf(fd, client);
	if (ret < 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
                           "%s: connect closed from fd %u", __FUNCTION__, fd);
		return (-2);		
	}
	else if (ret > 0) {
		return (0);
	}
    
	for (;;) {
		decode_server_proto(client,count);		
		ret = remove_one_buf(client);
		assert(ret >= 0);
		if (ret == 0)
			break;
	}

    
    execute_test_step( count );
    
    return (0);
}

static int userinit( void )
{
    int i;
    int ret = 0;
    testuser *user_p = NULL;
	struct sockaddr_in sin;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(conn_port);
    if( servernum == 1 )
        ret = evutil_inet_pton(AF_INET, "192.168.1.113", &sin.sin_addr);
    else
        ret = evutil_inet_pton(AF_INET, SERVER_IP , &sin.sin_addr);
	if (ret != 1)
    {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "evutil_inet_pton failed[%d]", ret);
		goto done;
	}

    for(i = 0; i < Id_Count ; i++ )
    {

        user_p = (testuser *) malloc ( sizeof ( testuser ) );
        memset( user_p, 0x0, sizeof ( testuser ) );

        if( user_p == NULL )
        {
            log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,"malloc space for user fail %d", i);
            break;
        }

        usergroup[i] = user_p;
        usergroup[i]->userid = Id_Base + i;
        usergroup[i]->testid = TID_NOT_START;
        
        //Set status as init after create count
        usergroup[i]->userstate = USER_INIT;
        usergroup[i]->client_maps.cb_recv = testtool_cb_conn_recv;
        ret = game_add_connect_event((struct sockaddr *)&sin, sizeof(sin),
                                     &(usergroup[i]->client_maps));
        usergroup[i]->client_maps.fd = ret;

        log4c_category_log(mycat, LOG4C_PRIORITY_TRACE, "loop i = %d, sizeof testuser is %d ", i,
                           sizeof ( testuser ));
        log4c_category_log(mycat, LOG4C_PRIORITY_TRACE, "user id = %d, user address is %p",
                           usergroup[i]->userid, usergroup[ i ]);
        if (ret > 0)
        {
            log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "connect to conn srv success[%d]",
                               ret);
        }
        else
        {
            log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "connect to conn srv failed[%d]", errno);
            goto done;
        }
    }

        
    return 0;

done:
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "testtool stoped[%d]", ret);
	return (ret);
        
 }
/*
  static ttest_init( tstep ** ttlist)
  {
  return 0;

  }
*/
static int CreateUserGroup( void )
{
    int i;
    int ret = 0;
    testuser *user_p = NULL;
	struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(conn_port);
    if( servernum == 1 )
        ret = evutil_inet_pton(AF_INET, "192.168.1.113", &sin.sin_addr);
    else
        ret = evutil_inet_pton(AF_INET, SERVER_IP, &sin.sin_addr);
	if (ret != 1)
    {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "evutil_inet_pton failed[%d]", ret);
		goto done;
	}

    for(i = 0; i < Id_Count ; i++ )
    {
        user_p = (testuser *) malloc ( sizeof ( testuser ) );
        if( user_p == NULL )
        {
            log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,"malloc space for user fail %d", i);
            break;
        }
        memset(( testuser *) user_p, 0x0, sizeof ( testuser ) );
        
        usergroup[i] = user_p;
        usergroup[i]->userid = Id_Base + i;
        usergroup[i]->testid = TID_NOT_START;
        
        //Set status as init after create count
        usergroup[i]->userstate = USER_UNEXIT;
        usergroup[i]->client_maps.cb_recv = testtool_cb_conn_recv;
        ret = game_add_connect_event((struct sockaddr *)&sin, sizeof(sin),
                                     (CLIENT_MAP *)&(usergroup[i]->client_maps));
        usergroup[i]->client_maps.fd = ret;

        log4c_category_log(mycat, LOG4C_PRIORITY_TRACE, "loop i = %d, sizeof testuser is %d !", i,
                           sizeof ( testuser ));
        log4c_category_log(mycat, LOG4C_PRIORITY_TRACE, "user id = %d, user address is %p !",
                           usergroup[i]->userid, usergroup[ i ]);
        if (ret > 0)
        {
            log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "connect to conn srv success[%d] !",
                               ret);
            //Create test count
            log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "Start create new acc userid %d!",
                               usergroup[i]->userid);
            send_createrole_request(usergroup[i]->client_maps.fd, usergroup[i]->userid);
        }
        else
        {
            log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "connect to conn srv failed[%d]!", errno);
            goto done;
        }
    }

        
    return 0;

done:
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "testtool stoped[%d]", ret);
	return (ret);
        
}

int main(int argc, char **argv)
{
	int i;
	int ret = 0;
    int createflag = 0;
	FILE *file;
	char *line;

        
    if (argc > 2)
    {
        Id_Base  = atoi(argv[1]);
        printf("get base id %d \n", Id_Base );
        Id_Count  = atoi(argv[2]);
        printf("get Id count %d \n", Id_Count );
        STATISTICS_TIMER = atoi(argv[3]);
        printf("get statistics timer %d \n", STATISTICS_TIMER );
    }
    else
    {
        printf( "argument is less!!!\n" );
        printf( "Example ./testtool **( id_base) ***( id_count )  **( timer )  -s 1( 113 )\n" );
        printf( "Example ./testtool **( id_base) ***( id_count )  **( timer )  (using default server )\n" );        
        return 0;
    }

    file = fopen("../server_info.ini", "r");
	if (!file) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "open server_info.ini failed[%d]", errno);				
		ret = -1;
		goto done;
	}
	line = get_first_key(file, "conn_srv_listen_port");
	conn_port = atoi(get_value(line));

    file = fopen("../test_info.ini", "r");

    if (!file) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "open test_info.ini failed[%d]", errno);				
		ret = -1;
		goto done;
	}
	line = get_first_key(file, "test_step");
	teststeps = atoi(get_value(line));

	if (conn_port <= 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "config file wrong, no conn_srv_listen_port");
		ret = -1;
		return ret;
	}

    usergroup = malloc(sizeof(void *) * Id_Count);
    if(  usergroup == NULL )
    {
        log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
                           "malloc space for usergroup fail");
    }
    memset(usergroup, 0x0, sizeof (void *) * Id_Count);
        

	ret = log4c_init();
	if (ret != 0) {
		printf("log4c_init failed[%d]\n", ret);
		return (ret);
	}
	mycat = log4c_category_get("test.log");
	if (!mycat) {
		printf("log4c_category_get(\"six13log.log.app.application1\"); failed");
		return (0);
	}

    srand((unsigned )time( NULL ));
    
	for (i = 3; i < argc; ++i) {
		if (strcmp(argv[i], "-d") == 0) {
			change_to_deamon();
            continue;
		}
        if (strcmp(argv[i], "-c") == 0) {
			createflag = 1;
            continue;
		}
        if (strcmp(argv[i], "-s") == 0) {
			servernum = atoi(argv[ ++i ]);
            continue;
		}

	}
	
    log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "testtool run");
    
	ret = game_event_init();
	if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "game_event_init failed[%d]", ret);		
		goto done;
	}
    
    ret = test_step_init(  );
    if (ret != 0) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "test_step_init failed[%d]", ret);		
		goto done;
	}
	
    if(createflag > 0)
    {
		log4c_category_log(mycat, LOG4C_PRIORITY_NOTICE, "CreateUserGroup Starting");
        CreateUserGroup();
        goto wait;
    }

    userinit();
    log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "userinit finished");		

    for (i = 0; i < Id_Count; ++i)
    {
        log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "send_login_request start", ret);
        send_login_request(usergroup[i]->client_maps.fd, usergroup[i]->userid);
    }
        
    log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "send_login_request end", ret);

wait:
    signal(SIGUSR1, signal_handler);  
    set_timer(STATISTICS_TIMER);  
    ret = event_base_loop(base, 0);
    log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "event_base_loop stoped[%d]", ret);
done:
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "testtool stoped[%d]", ret);
	return (ret);
}

