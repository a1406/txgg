#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>

int main(void);

int main()
{
	int s;
	int optval;
	socklen_t optlen = sizeof(optval);

		/* Create the socket */
	if((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket()");
		exit(EXIT_FAILURE);
	}

//	close(s);

//	exit(EXIT_SUCCESS);

    int new_fd;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	unsigned int sin_size,myport,listnum;
	myport = 9600; 
	listnum = 10;

	my_addr.sin_family = PF_INET;
    my_addr.sin_port = htons(myport);    
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero), 0);

	if(bind(s,(struct sockaddr *)&my_addr,sizeof(struct sockaddr)) == -1)
	{
		perror("bind is error\n");
		exit(1);
	}

    if(listen(s,listnum) == -1)
	{
		perror("listen is error/n");
		exit(1);
	}
	printf("start to listen\n");


	sin_size = sizeof(struct sockaddr_in);

	if((new_fd = accept(s,(struct sockaddr *)&their_addr,&sin_size)) == -1)
	{
		perror("accept is error\n");
		return (-1);
	}
//	printf("server:got connection from %s/n",inet_ntoa(their_addr.sin_addr));

	unsigned int old_time = time(NULL);

		/* Check the status for the keepalive option */
	if(getsockopt(new_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
		perror("getsockopt()");
		close(s);
		exit(EXIT_FAILURE);
	}
	printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));

		/* Set the option active */
	optval = 1;
	optlen = sizeof(optval);
	if(setsockopt(new_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
		perror("setsockopt()");
		close(s);
		exit(EXIT_FAILURE);
	}
	printf("SO_KEEPALIVE set on socket\n");

		/* Check the status again */
	if(getsockopt(new_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
		perror("getsockopt()");
		close(s);
		exit(EXIT_FAILURE);
	}
	printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));

	
	char buf[100];
	int ret = read(new_fd, buf, 100);

	printf("ret = %d, err = %d\n", ret, errno);

	unsigned int new_time = time(NULL);
	printf("time escape is = %lu\n", new_time - old_time);
	return (0);
}
