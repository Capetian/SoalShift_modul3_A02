#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define PORT 8081

pthread_t tid[2];
static int *stock;
int running = 1;


void* conn(void* args);

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    if (bind(server_fd, (struct sockaddr *)&address, (socklen_t) addrlen)<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }  
    
    if (listen(server_fd, 1) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
   
    key_t key = 1234;

    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    stock = shmat(shmid, NULL, 0);


    while( (new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) ){
        pthread_create(&tid[0], NULL, &conn, (void*) &new_socket);
        if (running == 0) {
            close(new_socket);
            close(server_fd);
            break;
        }
        
    }
    shmdt(stock);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

void* conn(void* args)
{
    int valread, new_socket = *(int*) args;
    char buffer[1024] = {0};
    valread = read( new_socket , buffer, 1024);
    if (valread > 0) {
        if (strcmp(buffer,"exit") == 0)
        {
            running = 0;
            return 0;
        }
        else if (strcmp(buffer,"beli") == 0) {
            int temp = *stock;
            char *hello = temp > 0 ? "transaksi berhasil" : "transaksi gagal";
            if (strstr(hello, "berhasil")) {
                *stock= *stock - 1;
            }
            send(new_socket , hello , strlen(hello) , 0 );
        }  
    }
}