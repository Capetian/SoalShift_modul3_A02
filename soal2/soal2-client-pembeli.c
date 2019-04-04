#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 7011
  
int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int sock = 0, reader;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    char message[1024] = "Establishing Connection.";
    char mailbox[1024] = {0};
    send(sock , message , strlen(message) , 0 );
    read( sock , mailbox, 1024);
    printf("%s\n", mailbox );
    if (strstr(mailbox,"Busy")) 
        return 0;
    while(1){
            
        memset(mailbox,0,1024);
        memset(message,0,1024);
        scanf("%s", message);
        send(sock , message , strlen(message) , 0 );
        if (strcmp(message,"exit") == 0)
            break;
        reader = read( sock , mailbox, 1024);
        printf("%s\n",mailbox );

    }
    close(sock);
    return 0;

}