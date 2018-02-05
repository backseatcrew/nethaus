#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
    if(argc < 2) {
        printf("No port provided. Exiting..\n");
        exit(1);
    }

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
        printf("Could not open socket!!\n");


    struct sockaddr_in serverIp, clientIp;
    socklen_t lengthServerIp = sizeof(serverIp);
    memset(&serverIp, 0, lengthServerIp);   //frombzero to memset

    int port = atoi(argv[1]);

    serverIp.sin_family = AF_INET;
    serverIp.sin_addr.s_addr = INADDR_ANY;
    serverIp.sin_port = htons(port);

    if(bind(sock, (struct sockaddr *) &serverIp, sizeof(serverIp)) < 0)
        printf("Error attempting to bind!!\n");




    return 0;
}
