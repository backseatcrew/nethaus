struct payload {
    char * anyFileType;
    uint32_t ip;
    int port;

}*p;

struct ack {
    int sequence;
    int length;
}ack;

//establish handshake, send request with file (fullpath of server file location in request as string or array of chars), have a fixed length (1024 or something), if file path doesn't exist terminate (ack to close connection), read file in chunks of 1 kb, packatize it and send it, open file and seralize
//payload should include packet size as well as src destination
struct payload2 {
    unsigned char length;
    unsigned char type;
    unsigned char *data;
};
//ack sequence number, length of packet, (ack request) 32 bit number never cycle back
//two structures one for the ack for different packet types ( data and ack)
//2 meg jpg for test and open file and looks same we're good
//chars and bites dont need hton or ntoh, only any type of int types!!
//noth to deserialize
struct header {
    uint32_t src;
    uint32_t dest;
    int port;
    struct payload2 data[8];
} header;
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
char serialize() {
    char b[sizeof(p->anyFileType) + sizeof(p->ip) + sizeof(p->port)];
    int off = 0;
    memcpy(b, &p->anyFileType, sizeof(p->anyFileType));
    off = sizeof(p->anyFileType);
    memcpy(b+off, &p->ip, sizeof(p->ip));
    off += sizeof(p->port);
    memcpy(b+off, &p->port, sizeof(p->port));
    return *b;
}

char serialize2() {
    char b[sizeof(uint32_t)*2 + sizeof(int) + sizeof (struct payload2)*8 ];
    int off = 0;

}
////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
struct payload deserialize (char serializedData) {
    struct payload data;
    memcpy(&data.anyFileType, &serializedData, sizeof(data.anyFileType));
    memcpy(&data.port, &serializedData + sizeof(&data.ip), sizeof(data.ip));
    memcpy(&data.ip, &serializedData +sizeof(&data.ip)+ sizeof(&data.port), sizeof(data.port));
    return data;
}
/*
struct payload deserialize2 (char serializedData) {
    struct payload data;
    memcpy(&data.anyFileType, serializedData, sizeof data.anyFileType);
    memcpy(&data.ip, serializedData + data.anyFileType, sizeof data.ip);
    memcpy(&data.port, serializedData + sizeof data.anyFileType + sizeof data.ip, sizeof data.port);
    return data;
}
*/
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
