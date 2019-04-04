#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


int mutex = 0, step = 1;
pthread_t tid[6];
typedef struct s_arg{int n;} arg;

void* Step( void* args);

int main(int argc, char** argv) {

	int i,n;

    arg args[argc];
 
	for ( i = 0; i < 10 ; i++){
		pthread_create(&tid[i%2], NULL, &Step, NULL );
		if (i%2==1) {
		    int j;
		    for ( j = 0; j <2; j++){
			pthread_join (tid[j], NULL);
		    }
		    step++;
		    mutex = 0;
		}
        
	}
	pthread_exit(NULL);
	return 0;
}



void* Step( void* args)
{
    int  n = ++mutex;
    char dir[26] = "~/Documents/FolderProses";
    sprintf(dir,"%s%d",dir, n);
    if ( step == 1) {
        char step1[70] = "mkdir";
        sprintf(step1,"%s %s",step1, dir);
        system(step1);
        while(1){
            if (mutex == 2) {
                return 0;
            }
        }
    }
    
    char filen[30] = "SimpanProses";
    sprintf(filen,"%s%d.txt", filen, n);

    if ( step == 2) {
        char step2[70] = "ps -aux | head -n 11 >";
        sprintf(step2,"%s %s/%s", step2, dir, filen);
        system(step2);
        while(1){
            if (mutex == 2) {
                return 0;
            }
        }
    }


    char zip[30] = "KompresProses";
    sprintf(zip,"%s%d.zip", zip, n);

    if ( step == 3) {
        char step3[100] = "zip";
        sprintf(step3,"%s -j %s/%s %s/%s", step3, dir, zip, dir, filen);
        system(step3);
        while(1){
            if (mutex == 2) {
                return 0;
            }
        }
    }
    if ( step == 4) {
        char step4[70] = "rm";
        sprintf(step4,"%s %s/%s", step4, dir, filen);
        system(step4);
        while(1){
            if (mutex == 2) {
                return 0;
            }
        }
    }
    printf("Menunggu 15 detik untuk mengekstrak kembali \n");
    sleep(15);
    char step5[120] = "unzip";
    sprintf(step5,"%s %s/%s -d %s", step5, dir, zip, dir);
    system(step5);

}
