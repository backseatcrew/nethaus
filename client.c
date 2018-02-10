#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "sendrecieve.h"
#include <time.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <pthread.h>
//#include <unistd.h>

int main(int argc, char *argv[]) {
    if(argc < 3) {
        printf("Not enough arguments provided. Exiting..\n");
       exit(0);
    };

    int port = atoi(argv[2]);  //port
    int sock = socket(AF_INET, SOCK_DGRAM, 0); //socket
    struct sockaddr_in serverIp;
    socklen_t lengthServerIp = sizeof(serverIp);

    struct in_addr argvIp;  //ip address
    inet_pton(AF_INET, argv[1], &argvIp);
    struct hostent *server = gethostbyaddr(&argvIp, sizeof argvIp, AF_INET); //NOT DEPERICATED!

    if(server == NULL) {
        printf("Could not verify host. Exiting..\n");
        exit(0);
    };


    if(sock < 0)
        printf("Could not open socket!!\n");

    memset(&serverIp, 0, lengthServerIp);   //making serverIp NULL
    serverIp.sin_family = AF_INET;
    serverIp.sin_port = htons(port);
    serverIp.sin_addr.s_addr = inet_addr(argv[1]); //placing in IP address

    memcpy((char *) server->h_addr_list[0], (struct sockaddr*)&serverIp.sin_addr.s_addr, server->h_length); //from bcopy to memcopy



    p = malloc(sizeof(struct payload));
    p->port = port;   //header port
    inet_pton(AF_INET, argv[1], &p->ip);  //header ip
    p->anyFileType = &argv[3];

    struct serialize *dataSerialized;
    dataSerialized = malloc(sizeof (struct serialize));

    serialization(p,dataSerialized);

   clock_t start = clock();
   clock_t end = clock() - start;

   return 0;
}



