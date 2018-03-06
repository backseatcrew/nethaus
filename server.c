#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "sendrecieve.h"
#include <sys/time.h>
//#include <netdb.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <pthread.h>


int main(int argc, char * argv[]) {
    if(argc < 2){
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


    while(!feof(file)) {

        //READ FILE AND PREPARE PACKET
        fread(buffer, 1, 1024, file);
        memset(&recieved.data.data, 0, sizeof recieved.data.data);
        strcpy(sent.data.data, buffer);
        memset(&buffer, 0, sizeof buffer);
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


        char recvSerialized[fixedSized];

        //SEND PACKET
        while(1){
                sendto(sock, &b, sizeof b, 0, (struct sockaddr*)&from, fromLength);
                printf("%s", "Server Sent\n");



        //RECIEVE PACKET
                recieved_correct = recvfrom(sock, &recvSerialized, sizeof(recvSerialized), 0, (struct sockaddr*)&from, &fromLength);


        //DE-SERIALIZE
                uint16_t seq2;
                uint16_t leng2;
                uint16_t ack2;

                int off2 = 0;
                memcpy(&seq2, recvSerialized, sizeof(seq2));
                off2 += sizeof(seq2);
                memcpy(&leng2, recvSerialized + off2, sizeof(leng2));
                off2 += sizeof(leng2);
                memcpy(&ack2, recvSerialized + off2, sizeof(ack2));
                off2 += sizeof(ack2);
                memcpy(&recieved.data.data, recvSerialized +off2, sizeof(recieved.data.data));


                recieved.sequence = ntohs(seq2);
                recieved.length = ntohs(leng2);
                recieved.ack = ntohs(ack2);


                printf("%d", recieved.sequence);

            //ACK AND CONTINUE TO PROCESS FILE BY QUITING RE-TRANSMITTION AND READING FILE
                if (recieved_correct > 0 && recieved.sequence == sequence_val){
                    break;
                }
        }
        ++sequence_val;
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

