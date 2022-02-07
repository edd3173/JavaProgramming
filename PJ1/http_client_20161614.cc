#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXDATASIZE 100000

// max number of bytes we can get at once

char* getPortNum(char* str) {
	char* sArr[10] = { NULL, }; int i = 0;
	char* dftPort = (char*)malloc(sizeof(char)*3);
	strcpy(dftPort, "80");
	char* ptr = strtok(str, ":");
	while (ptr != NULL) {
		sArr[i] = ptr;
		i++;
		ptr = strtok(NULL, "/");
	}
	
	if (sArr[1] != NULL) {
		//printf("PortStr : %s\n", sArr[1]);
		return sArr[1];
	}
	
	else
	{
		//printf("Default port str\n");
		return dftPort;
	}
}






char* getHostName(char* str) {
	char* sArr[10] = { NULL, }; int i = 0;
	if(strchr(str,':')!=NULL){ // exits ':'
		char* ptr = strtok(str, ":");
		while (ptr != NULL) {
			sArr[i] = ptr;
			i++;
			ptr = strtok(NULL, "/");
		}
		//printf("HostName : %s\n", sArr[0]);
		return sArr[0];
	}
	else{
		char* ptr=strtok(str, "/");
		sArr[i]=ptr;
		return sArr[0];
	}
}




char* getFileName(char* str) {
	char* dftFilename = (char*)malloc(sizeof(char)*2);
	strcpy(dftFilename,"/");

	char* str_copy=(char*)malloc(sizeof(char)*MAXDATASIZE);
	strcpy(str_copy, str);
	
	char* tmp=(char*)malloc(sizeof(char)*MAXDATASIZE);
	tmp=strtok(str_copy, "/");
	// str : localhost:5678/somedir/anotherfile.html
	// tmp : localhost:5678
	// we want to str - tmp
	char* ans=(char*)malloc(sizeof(char)*MAXDATASIZE);
	
	if(strchr(str,'/')==NULL){
		// must be default
		return dftFilename;
		
	}
	
	else{
		for(int i=(int)strlen(tmp);i<strlen(str);i++){
			ans[i-strlen(tmp)]=str[i];
		}
		return ans;
	}
}

char* getStatusCode(char* str){
	char* str_copy=(char*)malloc(sizeof(char)*MAXDATASIZE);
	strcpy(str_copy, str);
	char* tmp=strtok(str_copy, "\r\n");
	//printf("StatusCode? : [%s]\n",tmp);

	return tmp;
}

int checkHeader(char* str){
	char* str_copy=(char*)malloc(sizeof(char)*MAXDATASIZE);
	strcpy(str_copy, str);
	if(strstr(str_copy, "\r\n\r\n")==NULL)
		return -1;
	else
		return 0;
}

char* getContentLength(char* str){
	char* str_copy=(char*)malloc(sizeof(char)*MAXDATASIZE);
	strcpy(str_copy, str);
	char* noData=(char*)malloc(sizeof(char)*10);
	strcpy(noData, "NONE");
	char* ptr=(char*)malloc(sizeof(char)*MAXDATASIZE);
	//printf("str_copy : \n[%s]\n",str_copy);
	char* ans=(char*)malloc(sizeof(char)*8);
	if(strstr(str_copy, "Content-Length")!=NULL){
		ptr=strstr(str_copy,"Content-Length");
		char* tmp=strtok(ptr, "\r\n");
		//printf("tmp : [%s]\n",tmp);
		//tmp : Content-length: 2217

		int startIdx=0;
		// find the idx where number starts
		for(int i=0;i<=(int)strlen(tmp);i++){
			if(tmp[i]>='0' && tmp[i]<='9'){
				startIdx=i;
				break;
			}
		}
		//printf("startIdx : %d\n",startIdx);
		//printf("strlen : %d\n",(int)strlen(tmp));
		for(int i=startIdx;i<=(int)strlen(tmp);i++){
			ans[i-startIdx]=tmp[i];
		}

		//printf("ans : [%s]\n",ans);

		return ans;
	}
	else if (strstr(str_copy, "Content-length")!=NULL){
		ptr=strstr(str_copy,"Content-length");
		char* tmp=strtok(ptr, "\r\n");
        //printf("tmp : [%s]\n",tmp);
         //tmp : content-length: 646
	
		int startIdx=0;
        // find the idx where number starts
	         
		for(int i=0;i<=(int)strlen(tmp);i++){
			if(tmp[i]>='0' && tmp[i]<='9'){
				startIdx=i;
				break;
			}
		}
	    
		for(int i=startIdx;i<=(int)strlen(tmp);i++){
			ans[i-startIdx]=tmp[i];
		}
		//printf("ans : [%s]\n",ans);
		return ans;
	}
	else if (strstr(str_copy, "content-Length")!=NULL){
		ptr=strstr(str_copy,"content-Length");

		char* tmp=strtok(ptr, "\r\n");
		//printf("tmp : [%s]\n",tmp);
		//tmp : content-length: 646
		
		int startIdx=0;
		// find the idx where number starts
		for(int i=0;i<=(int)strlen(tmp);i++){
			if(tmp[i]>='0' && tmp[i]<='9'){
				startIdx=i;
				break;
			}
		}

		
		for(int i=startIdx;i<=(int)strlen(tmp);i++){
			ans[i-startIdx]=tmp[i];
		}

		//printf("ans : [%s]\n",ans);
		return ans;
	}
	else if(strstr(str_copy, "content-length")!=NULL){
		ptr=strstr(str_copy,"content-length");
		char* tmp=strtok(ptr, "\r\n");
		
		//printf("tmp : [%s]\n",tmp);
		
		//tmp : content-length: 2127

		int startIdx=0;
		
		// find the idx where number starts

		for(int i=0;i<=(int)strlen(tmp);i++){
			if(tmp[i]>='0' && tmp[i]<='9'){
				startIdx=i;
				break;
			}
		}

		for(int i=startIdx;i<=(int)strlen(tmp);i++){
			ans[i-startIdx]=tmp[i];
		}
		//printf("ans : [%s]\n",ans);
		return ans;
	
	}
	else if(strstr(str_copy,"CONTENT-LENGTH")!=NULL){
		ptr=strstr(str_copy, "CONTENT-LENGTH");
		char* tmp=strtok(ptr, "\r\n");
		int startIdx=0;

		// find the idx where number starts
		
		
		for(int i=0;i<=(int)strlen(tmp);i++){
			if(tmp[i]>='0' && tmp[i]<='9'){
				startIdx=i;
				break;
			}
		}

		for(int i=startIdx;i<=(int)strlen(tmp);i++){
			ans[i-startIdx]=tmp[i];
		}
		//printf("ans : [%s]\n",ans);
		return ans;
	}
	else{
		// no content length, return "NONE"
		return noData;
	}
}




int main(int argc, char* argv[]) {
	if(argc!=2){
		fprintf(stderr,"usage: http_client http://hostname[:port][/path/to/file]\n");
		exit(0);
	}
	
	char str[128];
	strcpy(str,argv[1]);
	//str[strlen(str) - 1] = '\0';
	//printf("your input : %s\n", str);
	
	if (strncmp(str, "http://", 7) != 0) {
		fprintf(stderr,"usage: http_client http://hostname[:port][/path/to/file]\n");
		exit(0);
	}
	
	char* buff = (char*)malloc(sizeof(char) * 128);
	
	// http://www.sogang.ac.kr
	
	for (int i = 7; i < strlen(str); i++)
		buff[i - 7] = str[i];

	
	// must put buff[16] = '\0'
	// strlen(str) : 24,
	// 16=24-7-1

	buff[strlen(str) - 7] = '\0';
//	printf("After http:// -> [%s]\n", buff);
	
	char* portBuff = (char*)malloc(sizeof(char) * 128);
	char* hostBuff = (char*)malloc(sizeof(char) * 128);
	char* fileBuff = (char*)malloc(sizeof(char) * 128);
	
	strcpy(portBuff, buff);
	strcpy(hostBuff, buff);
	strcpy(fileBuff, buff);
	

	
	char* portStr = getPortNum(portBuff);
	char* hostname = getHostName(hostBuff);
	char* filepath = getFileName(fileBuff);

	// check parsed datas
	//printf("hostname : %s\nportNumber:%s\nfilename: %s\n\n", hostname, portStr,filepath);


	int sockfd, numbytes;
	char clientBuf[MAXDATASIZE];
	char serverBuf[MAXDATASIZE];
	struct addrinfo hints, *servinfo;
	int rv;
	char s[INET_ADDRSTRLEN];
	int CONTENT_LENGTH=0;


	fd_set master, read_fds;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;


	
	// argv[1] : ipaddr - hostname
	// argv[2] : port number string - portStr
	// argv[3] : hints - hints
	if ((rv = getaddrinfo(hostname, portStr, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: Name or service not known\n"); 
		exit(0);
	}
	
	// make socket
	if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) { 
		fprintf(stderr, "stderr : client making socket"); 
		exit(0);
	}

	// Failed to connect
	if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) { 
		close(sockfd); 
		fprintf(stderr, "connect : Connection refused\n"); 
		exit(0);
	}
	


	//inet_ntop(servinfo->ai_family, &((struct sockaddr_in*)servinfo->ai_addr)->sin_addr, s, sizeof s); 
	//printf("client: connecting to %s\n", s);
	
	freeaddrinfo(servinfo); // all done with this structure


	 //1st lin
	        
	 //GET OBJECTPATH HTTP/1.1\r\n
	
	char* firstLine=(char*)malloc(sizeof(char)*128);	
	strcpy(firstLine, "GET ");
	strcat(firstLine, filepath);
	strcat(firstLine, " HTTP/1.1\r\n");
	
	//printf("firstline : [%s]\n",firstLine);


	
	//2nd line
	// HOST : hostname[:port]\r\n


	
	char* secondLine=(char*)malloc(sizeof(char)*128);

	strcpy(secondLine, "Host: ");
	strcat(secondLine, hostname);
	strcat(secondLine, ":");
	strcat(secondLine,portStr);
	strcat(secondLine,"\r\n");

	
	//printf("secondline : [%s]\n",secondLine);
	
	//3rd line
	// only \r\n
	
	char* thirdLine=(char*)malloc(sizeof(char)*3);
	strcpy(thirdLine,"\r\n");
	//printf("thirdline : [%s]\n",thirdLine);
	
	// assemble
	strcat(clientBuf,firstLine);
	strcat(clientBuf,secondLine);
	strcat(clientBuf,thirdLine);

	//printf("client sending msg of : \n[%s]\n",clientBuf);


	if(send(sockfd, clientBuf,strlen(clientBuf),0)==-1){
		perror("perror:send");
		close(sockfd);
		exit(1);
	}

	if((numbytes=recv(sockfd, serverBuf, MAXDATASIZE, 0))==-1){
		perror("perror:recv");
		close(sockfd);
		exit(1);
	}


	/*
	// if header is cut.
	if(checkHeader(serverBuf)==-1){
		printf("Need to call more recv to get header!\n");
		char* headerTemp=(char*)malloc(sizeof(char)*MAXDATASIZE);
		strcpy(headerTemp, serverBuf);

		while(checkHeader(serverBuf)!=0){
			// do this until we got end of header
		
			  // reset serverBuf
			memset(serverBuf, 0, (int)strlen(serverBuf)*sizeof(char));
			
			// try recv
			if((numbytes=recv(sockfd, serverBuf, MAXDATASIZE, 0))==-1){
				perror("perror:recv");
				close(sockfd);
				exit(1);				
			}
			strcat(headerTemp, serverBuf);
		}
		
		strcpy(serverBuf, headerTemp);
	}
	*/

	serverBuf[numbytes]='\0';
	//printf("\n\n\n*** Client received msg from server: *** \n[%s]\n",serverBuf);



	char* statusCode=getStatusCode(serverBuf);
	//printf("statusCode : [%s]\n",statusCode);

	char* contentLength=getContentLength(serverBuf);
	//printf("contentLength : [%s]\n",contentLength);

	CONTENT_LENGTH=atoi(contentLength);
	//printf("CONTENT_LENGTH : %d\n",CONTENT_LENGTH);
	//printf("numbytes : %d\n",numbytes);

	//show the first line of header
	// doesn't work with http://www.google.com.
	for(int i=0;i<strlen(serverBuf)-1;i++){
		if(serverBuf[i]=='\r' && serverBuf[i+1]=='\n')
			break;
		else
			printf("%c",serverBuf[i]);
	}
	printf("\n");
	

	// exit if content-length not specified
	if(strcmp(contentLength,"NONE")==0){ // no header
		fprintf(stderr,"Content-Length not specified\n");
		exit(0);	
	}










	
	// Write Content in file to [20161614].out

	char* dataBuff=(char*)malloc(sizeof(char)*MAXDATASIZE + 1);

	FILE* fp=fopen("20161614.out","w");
	int CUR_DATA_SIZE=numbytes;

	if(CUR_DATA_SIZE>CONTENT_LENGTH){
		// guess we need not to call further
		// so just write the file

		//printf("Only one call\n");
	
		// header ends with "\r\n\r\n"
		// find the start of idx in serverBuf, and save rest in dataBuff
		
		char* tmp=strstr(serverBuf,"\r\n\r\n");
		//char* tmp2=(char*)malloc(sizeof(char)*numbytes+1);
		for(int i=4;i<(int)strlen(tmp);i++){
			dataBuff[i-4]=tmp[i];
		}
		//printf("\n\n\n\n******BODY data : *****\n\n\n\n[%s]\n",dataBuff);

		//printf("\n\n\n*** Writing data to [20161614].out ... ***\n");
		fwrite(dataBuff,sizeof(char),strlen(dataBuff),fp);
		printf("%d bytes written to 20161614.out\n",CONTENT_LENGTH);
	}
	else{
		//printf("Need to call recv more\n");
		// call recv
		// then update CURDATASIZE and check if it's bigger than CONTENT_LENGTH
		// do it again, while CURDATASIZE>CONTENT_LENGTH
		
		
		// first try
		char* tmp=strstr(serverBuf, "\r\n\r\n");
		for(int i=4;i<(int)strlen(tmp);i++)
			dataBuff[i-4]=tmp[i];


		
		//NOT finished, so do it again and again
		while(CUR_DATA_SIZE<CONTENT_LENGTH){
			//printf("\nCalling extra recv\n");
		// we need to do this until CUR_DATA_SIZE >= CONTENT_LENGTH
		// which means, we got every info in CONTENT_LENGTH
		// after finish one cycle, CUR_DATA_SIZE+=numbytes
			

			// reset serverBuf
			memset(serverBuf, 0, (int)strlen(serverBuf)*sizeof(char));
			
			
			// try recv
			if((numbytes=recv(sockfd, serverBuf, MAXDATASIZE, 0))==-1){
				perror("perror:recv");
				close(sockfd);
				exit(1);
			}

			// if no error, strcat it to dataBuff
			strcat(dataBuff, serverBuf);

			// update CUR_DATA_SIZE
			CUR_DATA_SIZE+=numbytes;
		}

		//printf("\n\n\n\n******BODY data : *****\n\n\n\n[%s]\n",dataBuff);
		
		//printf("\n\n\n*** Writing data to [20161614.out] ... ***\n\n");
		fwrite(dataBuff, sizeof(char), sizeof(char)*(int)strlen(dataBuff),fp);
		printf("%d bytes written to 20161614.out\n",CONTENT_LENGTH);
	}


	close(sockfd);
	return 0;
}









