#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
//#include "sendrecieve.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <pthread.h>

void main(int argc, char *argv[]) {
    if(argc < 3) {
        printf("Not enough arguments. System exiting..");
        exit(1);
    }
    //hostname resolution
    struct in_addr hostIp;
    inet_pton(AF_INET,argv[1], &hostIp.s_addr);
    struct hostent *host = gethostbyaddr(&hostIp, sizeof hostIp, AF_INET);
    if(host == NULL){
        printf("Could not verify host. Exiting..\n");
        exit(1);
    }
    char hostName[256];
    gethostname(hostName, sizeof hostName);
    printf("Host Name: %s and host IP: %s\n", hostName, inet_ntoa(*((struct in_addr *)host->h_addr_list[0])));
    //socket creation
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0) printf("Could not open socket!!\n");
    //host to server setup
    struct sockaddr_in server, from;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    //memcpy(&server.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    //printf("%d\n",hostIp.s_addr);
    //printf("%s\n", inet_ntoa(hostIp));
    //printf("%s\n",inet_ntoa(server.sin_addr));
    //printf("%d", server.sin_addr.s_addr);
    //server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(atoi(argv[2]));
    //finding our own ip
    struct ifaddrs *networkInterfaces, *temp;
    getifaddrs(&networkInterfaces);
    temp = networkInterfaces;
    struct sockaddr_in *ourIPInfo;
    while(temp) {
        if(temp->ifa_addr && temp->ifa_addr->sa_family == AF_INET){
            ourIPInfo = (struct sockaddr_in *) temp->ifa_addr;
        }
        temp = temp->ifa_next;
    }
    uint32_t clientIP = ourIPInfo->sin_addr.s_addr; //found our IP
    //requesting file
    char filename[1024];
    strcpy(filename, argv[3]);
    //memcpy(filename, &argv[3], sizeof(argv[3]));
    unsigned int length = sizeof(struct sockaddr_in);
    int fileRequestSent = sendto(sock, filename, 1024,0,(struct sockaddr*)&server, sizeof(server));

    if(fileRequestSent < 0) {
        printf("Failed to send file request! Exiting..\n");
        exit(1);
    }
    int bufferSize = 1024;
    char buffer [bufferSize];
    //response from server regarding if the file exists
    int fileRequestResponse = recvfrom(sock,buffer,bufferSize,0,(struct sockaddr*)&from, &length);
    printf("%s", buffer);

    int stillReading = 1;
    while(stillReading == 1){

    }
}





