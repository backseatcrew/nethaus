#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <pthread.h>
//#include <unistd.h>

int main(int argc, char *argv[]) {

    struct in_addr argvIp;  //ip address
    inet_pton(AF_INET, argv[1], &argvIp);
    struct hostent *server = gethostbyaddr(&argvIp, sizeof argvIp, AF_INET); //NOT DEPERICATED!

    int sock = socket(AF_INET, SOCK_DGRAM, 0); //should this be named something specifically? check the professors example
    int port = atoi(argv[2]);  //port. Could delete this and just use argv[2] when port is used..
    struct sockaddr_in serverIp;
    socklen_t lengthServerIp = sizeof(serverIp);

    if(argc < 3) {
        printf("Not enough arguments provided. Exiting..\n");
        exit(0);
    }


    if(server == NULL) {
        printf("Could not verify host. Exiting..\n");
        exit(0);
    }


    if(sock < 0)
        printf("Could not open socket!!\n");

    memset(&serverIp, 0, lengthServerIp);   //frombzero to memset
    serverIp.sin_family = AF_INET;
    memcpy((char *) server->h_addr_list[0], (struct sockaddr*)&serverIp.sin_addr.s_addr, server->h_length); //from bcopy to memcopy
    serverIp.sin_port = htons(port);

   return 0;
}
