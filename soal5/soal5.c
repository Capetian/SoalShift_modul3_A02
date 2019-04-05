#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <ncurses.h>
#include <termios.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int health_stats=300;
int hunger_stats=200;
int hygiene_stats=100;
int food_left=0;
int bath_cooldown=20;
int enemy_health=100;
int food_stock=10;
int flag=1;
int regen_flag=1;
int death_flag=0;

void *hunger(void *arg)
{
    while(1)
    {
        if(flag==1)
        {
            sleep(10);
            hunger_stats-=5;
        }
    }
}

void *hygiene(void *arg)
{
    while(1)
    {
        if(flag==1)
        {
            sleep(30);
            hygiene_stats-=10;
        }
    }
}

void *regen(void *arg)
{
    while(1)
    {
        if(regen_flag==1)
        {
            sleep(10);
            health_stats+=5;
	    if(health_stats>=300)
	    {
		health_stats=300;
	    }
        }
    }
}

void *bath_timer(void *arg)
{
    while(1)
    {
	if(flag==1)
	{
	    if(bath_cooldown>0)
	    {	sleep(1);
            bath_cooldown--;
	    }
	}
    }
}

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

void battle_scene()
{
    char choice;
    flag=0;
    regen_flag=0;
    while(1)
    {

    if(enemy_health<=0)
    {
	printf("You Win\n");
	sleep(2);
        enemy_health=100;
        break;
    }
    printf("Battle Mode\n");
    printf("Monster’s Health : %d\n", health_stats);
    printf("Enemy’s Health : %d\n", enemy_health);
    printf("Choices :\n");
    printf("1. Attack\n");
    printf("2. Run\n");

    choice=mygetch();

    if(choice=='1')
            {
                    enemy_health-=20;
                    if(enemy_health>0);
                    {
                        health_stats-=20;
                        if(health_stats<=0)
                        {
                            system("clear");
                            death_flag=1;
                            break;
                        }
                    }
		    system("clear");
            }

            else if(choice=='2')
            {
                enemy_health=100;
		system("clear");
                break;
            }
    }
}

void shop_modo()
{
    key_t key=969;
    int *food_stock;
    int choice;
    int shmid=shmget(key,sizeof(int), IPC_CREAT | 0666);
    food_stock=shmat(shmid, NULL, 0);

    while(1)
    {
        system("clear");
        regen_flag=0;
        printf("Shop Mode\n");
        printf("Shop food stock : %d\n", *food_stock);
        printf("Your food stock : %d\n", food_left);
        printf("Choices\n");
        printf("1. Buy\n");
        printf("2. Back\n");

        choice=mygetch();
        if(choice=='1')
        {
            if(*food_stock>=5)
            {
                food_left+=5;
                *food_stock-=5;
            }
            else
            {
                system("clear");
                printf("Insufficient Stock\n");
                sleep(2);
            }
        }
        if(choice=='2')
        {
	    system("clear");
            break;
        }
    }

    shmdt(food_stock);
    shmctl(shmid, IPC_RMID, NULL);
}

void standby_modo()
{
	char choice;
        while(1)
        {
	flag=1;
	regen_flag=1;
	system("clear");
        if(death_flag==1)
        {
            printf("Game Over\n");
	    sleep(2);
            break;
        }
    	printf("Standby Mode\n");
    	printf("Health : %d\n", health_stats);
    	printf("Hunger : %d\n", hunger_stats);
    	printf("Hygiene : %d\n", hygiene_stats);
    	printf("Food left : %d\n", food_left);
        if(bath_cooldown>0)
        {
            printf("Bath will be ready in %d s\n", bath_cooldown);
        }
        else
        {
            printf("Bath is ready\n");
        }
    	printf("Choices : \n");
    	printf("1. Eat\n");
    	printf("2. Bath\n");
    	printf("3. Battle\n");
    	printf("4. Shop\n");
    	printf("5. Exit\n");

        if(hunger_stats<=0||hygiene_stats<=0)
        {
            printf("Game Over\n");
            break;
        }
        choice=mygetch();
        if(choice=='1')
        {
            if(food_left>0)
            {
                food_left-=1;
                hunger_stats+=15;
                if(hunger_stats>200)
                {
                    hunger_stats=200;
                }
            }
        }
        if(choice=='2')
        {
            if(bath_cooldown==0)
            {
                hygiene_stats+=30;
                bath_cooldown=20;
                if(hygiene_stats>100)
                {
                    hygiene_stats=100;
                }
            }
        }
        if(choice=='3')
        {
            system("clear");
            battle_scene();
        }
        if(choice=='4')
        {
            system("clear");
            shop_modo();
        }
        if(choice=='5')
        {
            break;
        }
        }
	system("clear");
}

int main()
{
    char name[30];
    pthread_t tid[4];

    system("clear");
    printf("Please Name Your Monster : ");
    scanf("%s", name);
    printf("Your Monster's name is %s\n", name);
    sleep(1);
    pthread_create(&(tid[0]), NULL, hunger, NULL);
    pthread_create(&(tid[1]), NULL, hygiene, NULL);
    pthread_create(&(tid[2]), NULL, regen, NULL);
    pthread_create(&(tid[3]), NULL, bath_timer, NULL);
    system("clear");
    standby_modo();
}

