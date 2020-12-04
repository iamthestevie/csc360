#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>

#include "monitor.h"

//pthread_mutex_t lock;

void *philosopher(void *i)
{
	while(1)
	{
		//variable representing philosopher 
		int self = *(int *) i;
		printf("\nPhilosopher %d is thinking for 10 secs",self);
		usleep(10);
		//philosopher take chopsticks
		pickup(self);
		printf("\nPhilosopher %d is eating for 10 secs",self);
		sleep(10);
		//philosopher release chopsticks
		putdown(self);
	}

}

int main()
{
	int i, pos[N];
	//one thread corresponding to each philosopher
	pthread_t thread[N];
	pthread_attr_t attr;

	//initilize semaphore and other variables
	initialization();

	pthread_attr_init(&attr);

	for (i = 0; i < N; i++) 
	{
		pos[i] = i;
		//create thread corresponding to each philosopher
		pthread_create(&thread[i], NULL,philosopher, (int *) &pos[i]);
	}
	for (i = 0; i < N; i++)
	{
		pthread_join(thread[i], NULL);
	}

	return 0;
}
