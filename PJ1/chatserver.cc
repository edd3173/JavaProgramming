#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAXDATASIZE 1000
#define BACKLOG 10

int main(int argc, char* argv[]){

	int sockfd;
	int new_fd;
	struct addrinfo hints, *servinfo;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	char s[INET_ADDRSTRLEN];
	int rv;

	fd_set master;
	fd_set read_fds;
	int fdmax;

	char buf[MAXDATASIZE];
	int numbytes;

	if(argc !=2){
		printf("usage: server portnum\n");
		exit(1);
	}

	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;

	rv=getaddrinfo(NULL,argv[1],&hints,&servinfo);
	sockfd=socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);

	freeaddrinfo(servinfo);

	listen(sockfd, BACKLOG);
	printf("server : waiting for connections on port %s...\n",argv[1]);

	FD_ZERO(&master); // initialization
	FD_SET(sockfd, &master);
	fdmax=sockfd;

	while(1){
		read_fds=master; // readfds copy
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL)==-1){
			perror("select");
			exit(1);
		}

		for(int i=0;i<=fdmax;i++){
			if(FD_ISSET(i,&read_fds)){
				if(i==sockfd){
					sin_size=sizeof their_addr;
					//blocking
					new_fd=accept(sockfd, (struct sockaddr*)&their_addr, &sin_size);
					if(new_fd==-1){
						perror("accept");
						exit(1);
					}
					else{
						FD_SET(new_fd, &master);
						if(new_fd>fdmax)
							fdmax=new_fd;
					}

					inet_ntop(their_addr.ss_family, &((struct sockaddr_in*)&their_addr)->sin_addr,s,sizeof s);
					printf("server: got connection from %s\n",s);
				}
				else{
					if((numbytes=recv(i,buf,sizeof buf, 0))<=0){
						if(numbytes==0)
							printf("selectserver: socket %d hung up\n",i);
						else
							perror("recv");
						close(i);
						FD_CLR(i,&master);
					}
					else{
						buf[numbytes]='\0';
						printf("server received : %s\n",buf);

						for(int j=0;j<=fdmax;j++){
							if(j==sockfd) continue; // waiting new client
							if(j==i) continue; // i : msg가온 client. 굳이 돌려보내줘야?
							if(!FD_ISSET(j,&master)) continue; // deactive
							printf("sending to socket %d\n",j);
							if(send(j,buf,strlen(buf),0)==-1)
								perror("send");
						}
					}
			}
		}
	}
}
	return 0;
}
