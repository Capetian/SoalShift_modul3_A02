#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define PORT 7011

pthread_t tid[2];
static int *stock;
int active = 0;

void* conn(void* args);

int main(int argc, char const *argv[]) {
    int main_sock, conn_sock;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

      
    if ((main_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(main_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    if (bind(main_sock, (struct sockaddr *)&address, (socklen_t) addrlen)<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }  
    
    if (listen(main_sock, 1) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
   
    key_t key = 404;

    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    stock = shmat(shmid, NULL, 0);
    *stock = 0;

    while( (conn_sock = accept(main_sock, (struct sockaddr *)&address, (socklen_t*)&addrlen)) ){
        if (active == 0) {

            pthread_create(&tid[0], NULL, &conn, (void*) &conn_sock);
            active++;
        }
        else
        {
            char busy[1024] = "Server Busy\n";
            send(conn_sock , busy , strlen(busy) , 0 );
            close(conn_sock);
        }
    }
    shmdt(stock);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

void* conn(void* args)
{
    int reader, conn_sock = *(int*) args;
    char mailbox[1024] = {0};
    char respond[1024] = "Connection Established!\n";
    write(conn_sock , respond , strlen(respond) );
    while ((reader = recv( conn_sock , mailbox, 1024, 0)) > 0) 
    {
        if (strcmp(mailbox,"exit") == 0)
            break;
        else if (strcmp(mailbox,"beli") == 0) {
            int temp = *stock;
            char *message = temp > 0 ? "Transaksi Berhasil." : "Transaksi Gagal.";
            if (strstr(message, "Berhasil"))
                *stock= *stock - 1;
            send(conn_sock , message , strlen(message) , 0 );
        } 
        memset(mailbox, 0,1024); 
    }
    
    active--;
    fflush(stdout);
    close(conn_sock);
    return 0;
}