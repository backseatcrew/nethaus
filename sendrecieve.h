struct payload {
    void * anyFileType;
    uint32_t ip;
    int port;

}*p;

struct payload2 {
    unsigned char length;
    unsigned char type;
    unsigned char *data;
};

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

struct payload deserialize2 (char serializedData) {
    struct payload data;
    memcpy(&data.anyFileType, serializedData, sizeof data.anyFileType);
    memcpy(&data.ip, serializedData + data.anyFileType, sizeof data.ip);
    memcpy(&data.port, serializedData + sizeof data.anyFileType + sizeof data.ip, sizeof data.port);
    return data;
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
