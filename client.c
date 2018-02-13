#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include "sendrecieve.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <pthread.h>

void main(int argc, char *argv[]) {

    if(argc < 3) {
        printf("Not enough arguments provided. Exiting..\n");
       exit(0);
    };

    //hostname portion. Seeing if we can resolve the server
    struct in_addr argvIp;
    inet_pton(AF_INET, argv[1], &argvIp.s_addr);
    struct hostent *serverHostName = gethostbyaddr(&argvIp, sizeof argvIp, AF_INET); //less deprecated then getbyhostname, and less confusing then its successor

    if(serverHostName == NULL) {
        printf("Could not verify host. Exiting..\n");
        exit(0);
    };

    char HostName[256];
    gethostname(HostName, sizeof(HostName));
    char * ipAddy = inet_ntoa(*((struct in_addr *)serverHostName->h_addr_list[0]));
    printf("Host Server: %s  and its IP: %s\n", HostName, ipAddy);


    //socket portion
    int sock = socket(AF_INET, SOCK_DGRAM, 0); //socket
    if(sock < 0)
        printf("Could not open socket!!\n");

    //setup portion..resolving ip of destination
    struct sockaddr_in server, incoming;
    memset(&server, 0, sizeof(server));   //making server var. NULL
    server.sin_family = AF_INET;    //setting it's attributes
    server.sin_addr.s_addr = inet_addr(argv[1]); //placing in IP address
    memcpy((char *) serverHostName->h_addr_list[0], (struct sockaddr*)&server.sin_addr.s_addr, serverHostName->h_length); //from bcopy to memcopy.
    server.sin_port = htons(atoi(argv[2]));

    //finding our own ip address looking at the linux.ab.cs.pdx.edu inteface devices, and making a assumption their all clones, and that the last device has the ip we need..
    struct ifaddrs *localInterfaceInfo, *temp;
    getifaddrs(&localInterfaceInfo);
    temp = localInterfaceInfo;
    struct sockaddr_in *ipInfo;
    while(temp) {
        if(temp->ifa_addr && temp->ifa_addr->sa_family ==AF_INET){
            ipInfo = (struct sockaddr_in *)temp->ifa_addr;
            printf("%s: %s\n", temp->ifa_name, inet_ntoa(ipInfo->sin_addr));
        }
        temp = temp->ifa_next;
    }
    uint32_t srcIp = ipInfo->sin_addr.s_addr;//our IP


    struct header headerObj;
    headerObj.src = srcIp;
    inet_pton(AF_INET, argv[1], headerObj.dest);
    headerObj.port = atoi(argv[2]);

    int i = 0;
    int j = 0;

    for(i = 0; i < 8; i++) {
        headerObj.data[i].data = malloc(8);
    }



  //  p = malloc(sizeof(struct payload));
  //  p->port = atoi(argv[2]);   //header port
   // inet_pton(AF_INET, argv[1], &p->ip);  //header ip
  //  p->anyFileType = &argv[3];
//    uint32_t destIp = &p->ip;

 //   unsigned char data [1];
 //   char * dataPointer = data;

//    unsigned char data  = serialize();
//    struct payload deserializedData = deserialize2(data);

//    printf("type a string: \n\n");
//    fflush(stdout);
//    printf("%d\n", deserializedData.port);

    //printf("%d\n", p->port);
  // clock_t start = clock();
   //clock_t end = clock() - start;

    //char buffer[1024];
    //strcpy(buffer, "HELLO THIS IS A SIMPLE TEST!");
    //sendto(sock, buffer, 1024, 0, (struct sockaddr*)&serverIp, sizeof(serverIp));
}



