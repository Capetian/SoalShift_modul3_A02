#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <pthread.h>

int mygetch(void)
{
	struct termios oldt,
	newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}


void main()
{
        key_t key = 1234;
        int *food_stock;
	    char choice;
        int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
        food_stock = shmat(shmid, NULL, 0);
        *food_stock = 10;
	while(1)
	{
		system("clear");
		printf("Shop\n");
		printf("Food stock : %d\n", *food_stock);
		printf("Choices : \n");
		printf("1. Restock\n");
		printf("2. Exit\n");

		choice=mygetch();

		if(choice=='1')
		{
		    *food_stock+=100;
		}

		if(choice=='2')
		{
		    break;
		}
	}
        shmdt(food_stock);
        shmctl(shmid, IPC_RMID, NULL);
}
