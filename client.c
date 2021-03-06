#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <libgen.h>
#include "sendrecieve.h"
#include <sys/time.h>
#include <pthread.h>
//#include <time.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <net/if.h>


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

    //get window size
    memset(&buffer, 0, sizeof buffer);
    recvfrom(sock, buffer, sizeof buffer, 0, (struct sockaddr*)&from, &length);
    int window_size = atoi(buffer);
    printf("%s", "Window Size is:\n ");
    printf("%d\n", window_size);


    //get timeout size
    memset(&buffer, 0, sizeof buffer);
    recvfrom(sock, buffer, sizeof buffer, 0, (struct sockaddr*)&from, &length);
    int retransmit_timeout = atoi(buffer);
    printf("%s", "Retransmit time is:\n ");
    printf("%d\n", window_size);


    //beginning transmission
    char * fileName;
    fileName = basename(argv[3]);
    FILE * file;
    file = fopen(fileName,"wb");


    struct Ack sent;
    struct Ack recieved;
    int recieved_correct;
    int sequence_val = 0;
    int fixedSized = sizeof (recieved);



    struct timeval timer;
    timer.tv_sec = 0;
    timer.tv_usec = 100000;
    setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&timer, sizeof(timer));


    //////  /////   //////  /////// ///////

    int sock2 = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock2 < 0) printf("Could not open socket!!\n");

    struct sockaddr_in from2;
    memset(&from2, 0, sizeof(from2));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]+1));
    server.sin_addr.s_addr = INADDR_ANY;
    //timer_create();
   // pthread_create();
    //pthread_mutex_init();


    //////  /////   //////  /////// ///////

    int acks_sent = 0;
    int acks_recv = 1;


    while(1) {
    //RECIEVE
         int sentSeqs[acks_recv];
         int recvSeqs[acks_recv];
        char recvSerialized[fixedSized * acks_recv];
        memset(&recvSerialized, 0, sizeof recvSerialized);

        for(int i = 0; i <acks_recv; ++i){
            recieved_correct=recvfrom(sock, &recvSerialized[i], sizeof(recvSerialized), 0, (struct sockaddr*)&from, &length);
            recvSeqs[i] = i;
            printf("%s", "Client Recieved\n");
        }

        for(int i = 0; i < acks_recv; ++i){
        //DESERIALIZATION
                    uint16_t seq2;
                    uint16_t leng2;
                    uint16_t ack2;

                    int off2 = 0;
                    memcpy(&seq2, &recvSerialized[i], sizeof(seq2));
                    off2 += sizeof(seq2);
                    memcpy(&leng2, &recvSerialized[i] + off2, sizeof(leng2));
                    off2 += sizeof(leng2);
                    memcpy(&ack2, &recvSerialized[i] + off2, sizeof(ack2));
                    off2 += sizeof(ack2);
                    memcpy(&recieved.data.data, &recvSerialized[i] +off2, sizeof(recieved.data.data));


                    recieved.sequence = ntohs(seq2);
                    recieved.length = ntohs(leng2);
                    recieved.ack = ntohs(ack2);

                    recvSeqs[i] = recieved.sequence;
                   // printf("%d\n", recieved.length);


        //ACK AND CONTINUE
                   if(strcmp(&recvSerialized[i], "Finished") == 0){
                    fflush(file);
                    fclose(file);
                    close(sock);
                    printf("%s", "Client End\n");
                    exit(0);
                }
                    if(recieved_correct >0 ){

        //WRITE TO FILE
                        strcpy(buffer, recieved.data.data);
                        fwrite(buffer,sizeof buffer, 1, file);
                        fflush(stdout);
                        printf("%s", "Client Recieved\n");
                        memset(&buffer, 0, sizeof buffer);
                        memset(&recieved.data.data, 0, sizeof recieved.data.data);

        //PREPARE PACKET
                        sent.sequence = sequence_val;
                        sent.ack = 0;
                        sent.length = sizeof(sent.sequence) + sizeof(sent.length) + sizeof(sent.ack) + sizeof(sent.data.data);

        //SERIALIZE
                        uint16_t seq = htons(sent.sequence);
                        uint16_t leng = htons(sent.length);
                        uint16_t ack = htons(sent.ack);

                        char b [sizeof(sent.sequence) + sizeof(sent.length) + sizeof(sent.ack) + sizeof(sent.data.data)];
                        int off = 0;
                        memcpy(b, &seq, sizeof(seq));
                        off = sizeof(seq);
                        memcpy(b+off, &leng, sizeof(leng));
                        off += sizeof(leng);
                        memcpy(b+off, &ack, sizeof(ack));
                        off += sizeof(ack);
                        memcpy(b+off, &sent.data.data, sizeof(sent.data.data));


                    sendto(sock, &b, sizeof b, 0, (struct sockaddr*)&server, sizeof(server));
                    //sendto(sock, filename, 1024,0,(struct sockaddr*)&server, sizeof(server));
                    printf("%s", "Client Sent\n");
                    sentSeqs[i] = recieved.sequence;


                if(acks_recv<window_size)
                    ++acks_sent;
                ++sequence_val;
            }
        }

    }
    return 0;
}





