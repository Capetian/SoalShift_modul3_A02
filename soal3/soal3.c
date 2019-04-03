#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

int agmal_wakeup_status = 0;
int iraj_spirit_status = 100;
int flag=1;
int count_agmal=0;
int count_iraj=0;
int bangun_agmal;
int tidur_iraj;

void *order_agmal(void *arg)
{
    while(1)
    {
        if(bangun_agmal==0)
        {
            sleep(10);
            bangun_agmal=1;
            count_iraj=0;
        }
    }
}

void *order_iraj(void *arg)
{
    while(1)
    {
        if(tidur_iraj==0)
        {
            sleep(10);
            tidur_iraj=1;
            count_agmal=0;
        }
    }
}

int main()
{
    pthread_t tid[2];
    char system_call[20];
    char storage;
    bangun_agmal=1;
    tidur_iraj=1;
    int thread;
    pthread_create(&(tid[0]), NULL, order_agmal, NULL);
    pthread_create(&(tid[1]), NULL, order_iraj, NULL);

    while(1)
    {
        if(flag==0)
        {
            break;
        }

        else if(flag==1)
        {
            if(count_agmal==3)
            {
                printf("Fitur Iraj Ayo Tidur disabled 10 s\n");
                tidur_iraj=0;
            }

            if(count_iraj==3)
            {
                printf("Agmal Ayo Bangun disabled 10 s\n");
                bangun_agmal=0;
            }

            scanf("%[^\n]s", system_call);
            scanf("%c", &storage);

            if(strcmp(system_call, "All Status")!=0 && strcmp(system_call, "Agmal Ayo Bangun")!=0 && strcmp(system_call, "Iraj Ayo Tidur")!=0)
            {
                printf("Input Salah\n");
            }

            else if(strcmp(system_call, "All Status")==0)
            {
                printf("Agmal Wakeup_Status = %d\n", agmal_wakeup_status);
                printf("Iraj Spirit_Status = %d\n", iraj_spirit_status);
            }

            else if(strcmp(system_call, "Agmal Ayo Bangun")==0)
            {
                if(bangun_agmal==1)
                {
                    agmal_wakeup_status+=15;
                    if(agmal_wakeup_status>=100)
                    {
                        printf("Agmal Terbangun,mereka bangun pagi dan berolahraga\n");
                        flag=0;
                    }
                    count_agmal++;
                }
            }

            else if(strcmp(system_call, "Iraj Ayo Tidur")==0)
            {
                if(tidur_iraj==1)
                {
                    iraj_spirit_status-=20;
                    if(iraj_spirit_status<=0)
                    {
                        printf("Iraj ikut tidur, dan bangun kesiangan bersama Agmal\n");
                        flag=0;
                    }
                    count_iraj++;
                }
            }
        }
        system_call[0]='\0';
    }

    return 0;
}
