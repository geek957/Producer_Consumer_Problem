#include <stdio.h>
#include <string.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

int size;
int threads=0;
int write,read,buffer[1000];

typedef struct _rwlocks
{
	sem_t accesslock;
	sem_t writelock;
	int no_of_readers;
	int flag_to_write;
}rwlocks;

rwlocks rw[1000];


void rwlocks_initialise(rwlocks *rw)
{
	sem_init(&rw->accesslock,0,1);
	rw->flag_to_write=0;
	sem_init(&rw->writelock,0,1);
	rw->no_of_readers=0;
}

void *reader()
{
	int i;
	int read=0;
	for(i=size;i>0;i--)
	{
		sem_wait(&(rw[size-i].accesslock));
		if(rw[size-i].flag_to_write==1)
		{
			rw[size-i].no_of_readers++;
			if(rw[size-i].no_of_readers==1)
				sem_wait(&(rw[size-i].writelock));
			
			printf("\nValue read from %d location is %d by reader%d\n",size-i,buffer[read],rw[size-i].no_of_readers);
			read=(read+1)%size;
			if(rw[size-i].no_of_readers % size == 0)
			{
				sem_post(&(rw[size-i].writelock));
				rw[size-i].flag_to_write=0;
			}
		}
		sem_post(&(rw[size-i].accesslock));
	}
}



void *writer()
{
	int i;
	int write=0;
	sleep(1);
	for(i=size;i>0;i--)
	{
		sem_wait(&(rw[size-i].writelock));
		//sem_wait(&(rw[size-i].accesslock));
		buffer[write]=rand();
		rw[size-i].flag_to_write=1;
		
		printf("\nValue written at %d location is %d" ,size-i,buffer[write]);
		write=(write+1)%size;
		//sem_post(&(rw[size-i].accesslock));
		sem_post(&(rw[size-i].writelock));
	}
}


int main()
{
	int i,n;
	printf("Enter total number of reader:\t");
	scanf("%d",&n);
	size=n;
	pthread_t wrt;
	pthread_t reads[n];
	for (i=0;i<n;i++)
	{
		rwlocks_initialise(&rw[i]);
	}
	pthread_create(&wrt,NULL,writer,NULL);
	for(i=0;i<n;i++)
	{
		pthread_create(&reads[i],NULL,reader,NULL);
	}
	for(i=0;i<n;i++)
		pthread_join(reads[i],NULL);
	pthread_join(wrt,NULL);
	return 0;
}