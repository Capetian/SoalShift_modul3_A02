#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int sig;
int mutex = -1;
pthread_t tid[100];
int bucket[100];
typedef struct s_arg{int n;} arg;

void* factorial( void* args)
{
    arg *data = (arg*) args;
    int  i, n = data->n;
    unsigned long long  res = 1;

    for( i = n; i > 1; i--)
    {
        res = res * i;
    }
    while(mutex != n){
    }
    
    i = 0;
    while(i++ < bucket[n]){
        printf("%d! = %llu\n", n, res);   
    }

}

int main(int argc, char** argv) {

	int i,n;

    arg args[argc];
 
	for (int i=1; i < argc ; i++){
        n = atoi(argv[i]);        
        bucket[n]++;
        if (bucket[n] == 1) {
            args[i].n = n;
            pthread_create(&tid[n], NULL, &factorial,(void *) &args[i]);
        } 
	}

	for ( mutex = 0; mutex < 100; mutex++){
        pthread_join (tid[mutex], NULL);
	}

	pthread_exit(NULL);
	return 0;
}
