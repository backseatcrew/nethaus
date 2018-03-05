#define DATA_SIZE 1024

struct Data {

    char data[DATA_SIZE];

} data;

struct Ack {
    int sequence;
    int length;
    int ack;
    struct Data data;
}ack;


char serialize(struct Ack sent) {
    uint16_t seq = htons(sent.sequence);
    uint16_t leng = htons(sent.length);
    uint16_t ack = htons(sent.ack);

    char b[sizeof(seq) + sizeof(leng) + sizeof(ack) + sizeof(sent.data.data)];
    int off = 0;
    memcpy(b, &seq, sizeof(seq));
    off = sizeof(seq);
    memcpy(b+off, &leng, sizeof(leng));
    off += sizeof(leng);
    memcpy(b+off, &ack, sizeof(ack));
    off += sizeof(sent.data.data);
    memcpy(b+off, &sent.data.data, sizeof(sent.data.data));
    return *b;
}

struct Ack deserialize (char* serializedData) {
    struct Ack recieved;

    uint16_t seq;
    uint16_t leng;
    uint16_t ack;

    memcpy(&seq, &serializedData, sizeof(seq));
    memcpy(&leng, &serializedData + sizeof(seq), sizeof(leng));
    memcpy(&ack, &serializedData +sizeof(seq)+ sizeof(leng)+sizeof(leng), sizeof(ack));
    int offset = sizeof(serializedData) - sizeof(seq)+ sizeof(leng)+sizeof(leng)+ sizeof(ack);
    memcpy(&recieved.data.data, &serializedData +sizeof(seq)+ sizeof(leng)+sizeof(ack), offset);

    recieved.sequence = ntohs(seq);
    recieved.length = ntohs(leng);
    recieved.ack = ntohs(ack);
    return recieved;
}
/*
establish handshake, send request with file (fullpath of server file location in request as string or array of chars), have a fixed length (1024 or something), if file path doesn't exist terminate (ack to close connection), read file in chunks of 1 kb, packatize it and send it, open file and seralize
payload should include packet size as well as src destination
*/

/*
ack sequence number, length of packet, (ack request) 32 bit number never cycle back
two structures one for the ack for different packet types ( data and ack)
2 meg jpg for test and open file and looks same we're good
chars and bites dont need hton or ntoh, only any type of int types!!
noth to deserialize
*/

/*
algorithm notes:
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
