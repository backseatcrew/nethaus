#define DATA_SIZE 1024

struct Data {

    char data[DATA_SIZE];

} data;

struct Ack {
    uint16_t sequence;
    uint16_t length;
    uint16_t ack;
    struct Data data;
}ack;


char serialize(struct Ack sent) {
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
    return *b;
}

struct Ack deserialize (char* serializedData) {
    struct Ack recieved;

    uint16_t seq2;
    uint16_t leng2;
    uint16_t ack2;

    int off2 = 0;
    memcpy(&seq2, serializedData, sizeof(seq2));
    off2 += sizeof(seq2);
    memcpy(&leng2, serializedData + off2, sizeof(leng2));
    off2 += sizeof(leng2);
    memcpy(&ack2, serializedData + off2, sizeof(ack2));
    //int offset = sizeof(serializedData) - sizeof(seq2)+ sizeof(leng2)+ sizeof(ack2);
    off2 += sizeof(ack2);
    memcpy(&recieved.data.data, serializedData +off2, sizeof(recieved.data.data));


    recieved.sequence = ntohs(seq2);
    recieved.length = ntohs(leng2);
    recieved.ack = ntohs(ack2);

    return recieved;
}

