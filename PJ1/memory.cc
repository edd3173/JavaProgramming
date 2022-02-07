#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char* getPortNum(char* str) {
	char* sArr[10] = { NULL, }; int i = 0;
	char dftPort[3] = "80";
	char* ptr = strtok(str, ":");
	while (ptr != NULL) {
		sArr[i] = ptr;
		i++;
		ptr = strtok(NULL, "/");
	}
	if (sArr[1] != NULL) {
		printf("PortStr : %s\n", sArr[1]);
		return sArr[1];
	}
	else
	{
		printf("Default port str\n");
		return dftPort;
	}
}


char* getHostName(char* str) {
	char* sArr[10] = { NULL, }; int i = 0;
	char* ptr = strtok(str, ":");
	while (ptr != NULL) {
		sArr[i] = ptr;
		i++;
		ptr = strtok(NULL, "/");
	}
	//printf("HostName : %s\n", sArr[0]);
	return sArr[0];
}

char* getFileName(char* str) {

}

int main() {
	char str[128];
	fgets(str, strlen(str), stdin);
	str[strlen(str) - 1] = '\0';
	printf("your input : %s\n", str);
	if (strncmp(str, "http://", 7) != 0) {
		fprintf(stderr, "wrong format!\n");
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
	
	printf("After http:// -> [%s]\n", buff);

	int portNum = 80;
	char* portStr = getPortNum(buff);
	char* hostname = getHostName(buff);
	//char* filename = getFileName(buff);

	if (strcmp(portStr, "80") != 0) {
		// need to update portNum
		portNum = atoi(portStr);
	}

	//printf("hostname : %s\nportNumber:%s\n", hostname, portStr);
	printf("hostname : %s\nportNumber:%d\n", hostname, portNum);

}