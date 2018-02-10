#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#define PACKET_SIZE 1

struct payload {

    int port;
    struct in_addr ip;
    void * anyFileType;

}*p;

struct serialize {
 void * serialized;
 int byteSize;
 size_t payloadSize;
};


void makeRoom(struct serialize * data, size_t sizeOfData) {
    if((data->byteSize + sizeOfData) > data->payloadSize) {
        data->serialized = realloc(data->serialized, data->payloadSize * 2);
        data->payloadSize *= 2;
    }
    else{
        return;
    }
}

void serializePort(int port, struct serialize * data){
    port = htons(port);
    makeRoom(data, sizeof(int));
    memcpy(((char *)data->serialized) + data->byteSize, &port, sizeof(int));
    data->byteSize += sizeof(int);
}

void serializeIP(struct in_addr ip, struct serialize *data) {
    makeRoom(data, sizeof(ip));
    memcpy(((char *)data->serialized) + data->byteSize, &ip, sizeof(ip));
    data->byteSize += sizeof(ip);
}

void serializeFile(void * anyFileType, struct serialize * data) {
    makeRoom(data, sizeof(anyFileType));
    memcpy(((char *)data->serialized) + data->byteSize, &anyFileType, sizeof(*anyFileType));
    data->byteSize += sizeof(*anyFileType);
}

void serialization(struct payload *pay, struct serialize * data) {
    serializePort(pay->port, data);
    serializeIP(pay->ip, data);
    serializeFile(pay->anyFileType, data);
}

/*algorithm notes:
timer = round robin on client via loop checking..check in some duration
server send ack = 0 for fail and 1 for success

use buffer length for checksum size check if neccessary

void * any file type

int sizeof for payload completion check. Else failure! used in conjunction with payload complete

serialize data via lecture 2 app layer 1 slide 32

should not need sequence number since we're doing ack's

1 kb in size should be packet size..not 32!

remember to include packet size for packet in struct! may use buffer as another argument

send as char? why not void??

ALGORITHM VIA PROFESSOR:
open packet....ack packet...data packet...ack data packet...request for file...source...then above algorithm for stop and wait!
*/
