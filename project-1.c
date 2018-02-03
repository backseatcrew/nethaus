#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

int main(int argc, char *argv[])
{
   // printf() displays the string inside quotation
   printf("Hello, World! WOOT WE'RE READY!");
   return 0;
}
