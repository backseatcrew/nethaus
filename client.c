#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <libgen.h>
#include "sendrecieve.h"
//#include <time.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <net/if.h>
//#include <pthread.h>

int main(int argc, char *argv[]) {

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
    server.sin_port = htons(atoi(argv[2]));


    //requesting file
    char filename[1024];
    strcpy(filename, argv[3]);
    unsigned int length = sizeof(struct sockaddr_in);
    int fileRequestSent = sendto(sock, filename, 1024,0,(struct sockaddr*)&server, sizeof(server));


    if(fileRequestSent < 0) {
        printf("Failed to send file request! Exiting..\n");
        exit(1);
    }

    //response from server regarding if the file exists
    int bufferSize = 1024;
    char buffer [bufferSize];
    memset(&buffer, 0, sizeof buffer);
    uint16_t fileSize;

    recvfrom(sock, buffer, sizeof buffer, 0, (struct sockaddr*)&from, &length);

    if(strcmp(buffer, "Finished") == 0) {
               printf("%s", "File does not exists. Exiting..");
               exit(0);
    }
    else{
        memcpy(&fileSize, buffer, sizeof(fileSize));
        uint16_t convertedFileSize = ntohs(fileSize);
        printf("%s", "File exists! File size is: ");
        printf("%d", convertedFileSize);
        printf("%s", " bytes.\n");
    }


    //beginning transmission
    char * fileName;
    fileName = basename(argv[3]);
    FILE * file;
    file = fopen(fileName,"wb");

    int fixedSized = sizeof(uint16_t) *3 + DATA_SIZE;

    while(1) {
        memset(&buffer, 0, sizeof buffer);
        recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&from, &length);

        if(strcmp(buffer, "Finished") == 0){
            fflush(file);
            fclose(file);
            close(sock);
            printf("%s", "Client End\n");
            exit(0);
        }

        fwrite(buffer,sizeof buffer, 1, file);
        fflush(stdout);
        printf("%s", "Client Recieved\n");
    }
    return 0;
}





