#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#define N 1000

int smp = -1;
pthread_t tid[N];
int bucket[N];
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
    while(smp != n){
    }
    
    i = 0;
    while(i++ < bucket[n]){
        printf("%d! = %llu\n", n, res);   
    }

}

int main(int argc, char** argv) {

	int i,n;

    arg args[argc];
    int MaxVal = -1;
	for (int i=1; i < argc ; i++){
        n = atoi(argv[i]);
        if ( n > MaxVal) MaxVal = n;       
        bucket[n]++;
        if (bucket[n] == 1) {
            args[i].n = n;
            pthread_create(&tid[n], NULL, &factorial,(void *) &args[i]);
        } 
	}

	for ( smp = 0; smp <= MaxVal; smp++){
        pthread_join (tid[smp], NULL);
	}

	pthread_exit(NULL);
	return 0;
}
