#include "stdlib.h"
#include "errno.h"
#include "string.h"

#include "sys/types.h"

#include "netinet/in.h"
#include "sys/socket.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

int main(int argc, char *argv[])
{
	int fd;
	struct tcp_info info;
	int len = sizeof(info);

    int sockfd, new_fd;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	unsigned int sin_size,myport,listnum;
	myport = 9600;  //绑定的端口号
	listnum = 10;

	if((sockfd = socket(PF_INET,SOCK_STREAM,0)) == -1 )
	{
		perror("socket is error/n;");
		exit(1);
	}
	my_addr.sin_family = PF_INET;

    my_addr.sin_port = htons(myport);      //如果填入0，系统将随机选择一个端口

	my_addr.sin_addr.s_addr = INADDR_ANY;  //填入本机IP地址
	bzero(&(my_addr.sin_zero),0);


	if(bind(sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr)) == -1)
	{
		perror("bind is error/n");
		exit(1);
	}

    if(listen(sockfd,listnum) == -1)
	{
		perror("listen is error/n");
		exit(1);
	}
	printf("start to listen/n");


	sin_size = sizeof(struct sockaddr_in);

	if((new_fd = accept(sockfd,(struct sockaddr *)&their_addr,&sin_size)) == -1)
	{
		perror("accept is error/n");
		return (-1);
	}
//	printf("server:got connection from %s/n",inet_ntoa(their_addr.sin_addr));

	
    while(1)
	{
		getsockopt(new_fd, SOL_TCP, TCP_INFO, &info, &len);		
		sleep(5);

	}	

	
	return (0);
}
