#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "sendrecieve.h"
#include <inttypes.h>
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
    int recieved_correct;
    int sequence_val = 0;


    int fixedSized = sizeof(uint16_t) *3 + DATA_SIZE;
    while(!feof(file)) {



        fread(buffer, 1, 1024, file);
        strcpy(sent.data.data, buffer);
        sent.sequence = sequence_val;
        sent.ack = 0;
        sent.length = sizeof(struct Ack);
        char sentSerialized = serialize(sent);

        char recvSerialized[fixedSized];

        sendto(sock, &sentSerialized, sizeof sentSerialized, 0, (struct sockaddr*)&from, fromLength);
        memset(&buffer, 0, sizeof buffer);
        printf("%s", "Server Sent\n");

        recieved_correct = recvfrom(sock, &recvSerialized, sizeof(recvSerialized), 0, (struct sockaddr*)&from, &fromLength);
        recieved = deserialize(recvSerialized);
        printf("%d", recieved.ack);
        if (recieved_correct > 0 ){

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

