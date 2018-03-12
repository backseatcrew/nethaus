#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "sendrecieve.h"
#include <sys/time.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
//#include <netdb.h>
//#include <sys/types.h>
//#include <sys/socket.h>



int main(int argc, char * argv[]) {
    if(argc < 3){
        printf("Not enough arguments. System exiting..");
        exit(0);
    }

    //socket creation
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0) printf("Could not open socket!!\n");
    //host to server setup
    struct sockaddr_in server, from;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = INADDR_ANY;
    //binding
    if(bind(sock,(struct sockaddr*) &server, sizeof(server)) < 0) printf("Error binding!!\n");

    //recieving handshake
    char buffer[1024];
    socklen_t fromLength = sizeof(from);
    recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&from, &fromLength);
    FILE * file;
    if((file = fopen(buffer, "rb"))) {
         printf("%s", "File exists!\n");
         fseek(file, 0, SEEK_END);
         uint16_t fileSize = ftell(file);
         fseek(file, 0, SEEK_SET);
         uint16_t convertedFileSize = htons(fileSize);
         sendto(sock, &convertedFileSize, sizeof convertedFileSize, 0, (struct sockaddr*)&from, fromLength);
    }
    else {
        printf("%s", "File does not exists. Exiting..");
        memset(&buffer, 0, sizeof buffer);
        strcpy(buffer, "Finished");
        sendto(sock, buffer, sizeof buffer, 0, (struct sockaddr*)&from, fromLength);
        exit(0);
    }

    //send window size
    memset(&buffer, 0, sizeof buffer);
    strcpy(buffer, argv[3]);
    sendto(sock, buffer, sizeof buffer, 0, (struct sockaddr*)&from, fromLength);
    //send timeout size
    memset(&buffer, 0, sizeof buffer);
    strcpy(buffer, argv[2]);
    sendto(sock, buffer, sizeof buffer, 0, (struct sockaddr*)&from, fromLength);

    //file exists. Beginning transmission
    struct Ack sent;
    struct Ack recieved;
    uint16_t recieved_correct;
    uint16_t sequence_val = 0;
    int fixedSized = sizeof(uint16_t) *3 + DATA_SIZE;


    struct timeval timer;
    timer.tv_sec = 0;
    timer.tv_usec = 100000;
    setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&timer, sizeof(timer));



    //// //// //// ////
    int sock2 = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock2 < 0) printf("Could not open socket!!\n");
    int retransmit_timeout = atoi(argv[2]);
    int window_size = atoi(argv[3]);


    struct sockaddr_in from2;
    memset(&from2, 0, sizeof(from2));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]+1));
    server.sin_addr.s_addr = INADDR_ANY;
   // time_t time;
    //time(time);
    //SIG stop;
    //timer_create();
    //timer_create(CLOCK_REALTIME,stop,&time);
   // pthread_create();
    //pthread_mutex_init();
    //// //// //// ////

    int acks_sent = 1;
    int acks_recv = 0;
    int recv_fail = 0;
    while(!feof(file)) {
        if(acks_sent > 1 && recv_fail == 1)
            acks_sent = acks_sent / 2;

         uint16_t seq;
         uint16_t leng;
         uint16_t ack;
         char b [(sizeof(sent.sequence) + sizeof(sent.length) + sizeof(sent.ack) + sizeof(sent.data.data))*acks_sent];

        //READ FILE AND PREPARE PACKET
        for(int i = 0; i < acks_sent; ++i) {
            fread(buffer, 1, 1024, file);
            memset(&recieved.data.data, 0, sizeof recieved.data.data);
            strcpy(sent.data.data, buffer);
            memset(&buffer, 0, sizeof buffer);
            sent.sequence = sequence_val;
            sent.ack = 0;
            sent.length = sizeof(sent.sequence) + sizeof(sent.length) + sizeof(sent.ack) + sizeof(sent.data.data);


            //SERIALIZE
            seq = htons(sent.sequence);
            leng = htons(sent.length);
            ack = htons(sent.ack);


            int off = 0;
            memcpy(&b[i], &seq, sizeof(seq));
            off = sizeof(seq);
            memcpy(&b[i]+off, &leng, sizeof(leng));
            off += sizeof(leng);
            memcpy(&b[i]+off, &ack, sizeof(ack));
            off += sizeof(ack);
            memcpy(&b[i]+off, &sent.data.data, sizeof(sent.data.data));
        }

        char recvSerialized[(fixedSized)*acks_sent];

        //SEND PACKET
        while(1){
            int sentSeqs[acks_sent];
            int recvSeqs[acks_sent];

            for(int i = 0; i <acks_sent; ++i) {
                    sentSeqs[i] = i;
                    sendto(sock, &b[i], sizeof b, 0, (struct sockaddr*)&from, fromLength);
                    printf("%s", "Server Sent\n");

            }

         for(int i = 0; i <acks_sent; ++i) {
            //RECIEVE PACKET
                    recieved_correct = recvfrom(sock, &recvSerialized, sizeof(recvSerialized), 0, (struct sockaddr*)&from, &fromLength);


            //DE-SERIALIZE
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


                    printf("%d", recieved.sequence);
                    recvSeqs[i] = recieved.sequence;
                //ACK AND CONTINUE TO PROCESS FILE BY QUITING RE-TRANSMITTION AND READING FILE
                    if (recieved_correct > 0 && sentSeqs[i] == recvSeqs[i]/*recieved.sequence == sequence_val*/){
                        ++acks_recv;
                        if(acks_recv == acks_sent){
                            break;
                        }
                    }
                    else{
                        recv_fail = 1;
                    }
            }
            if(acks_sent < window_size)
                ++acks_sent;
            ++sequence_val;

        }
    }

            memset(&buffer, 0, sizeof buffer);
            strcpy(buffer, "Finished");
            sendto(sock, buffer, sizeof buffer, 0, (struct sockaddr*)&from, fromLength);
            printf("%s", "Server End\n");

            fflush(file);
            fclose(file);
            close(sock);
            exit(0);

    return 0;
}

