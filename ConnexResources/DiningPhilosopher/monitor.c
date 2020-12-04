#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <semaphore.h>

#include "monitor.h"

//semaphores to implement monitor
sem_t mutex;
sem_t next;
//count varaible for philosophers waiting on semaphore next
int next_count = 0;

//implementing condition variable using semaphore
//semaphore and integer variable replacing condition variable
typedef struct
{
	sem_t sem;
	int count;
}condition;
condition x[N];

//state of each philosopher(THINKING, HUNGRY or EATING)
int state[N];

//wait on condition
void wait(int i)
{
	x[i].count++;
	if(next_count > 0)
	{
		sem_post(&next);
	}
	else
	{
		sem_post(&mutex);
	}
	sem_wait(&x[i].sem);
	x[i].count--;
}

//signal on condition
void signal(int i)
{
	if(x[i].count > 0)
	{
		next_count++;
		sem_post(&x[i].sem);
		sem_wait(&next);
		next_count--;
	}
}

// No need to protect test since it is only called by pickup and putdown, which have been protected.
void test(int i)
{
	if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
	{
		state[i] = EATING;

		signal(i);


	}
}

void pickup(int i)
{
	sem_wait(&mutex);
         // body of pickup
	state[i] = HUNGRY;
	test(i);
	if(state[i] != EATING) wait(i);

        //end of body of pickup

	if(next_count > 0)
	{
		sem_post(&next);
	}
	else
	{
		sem_post(&mutex);
	}
}

void putdown(int i)
{
	sem_wait(&mutex);
        // body of putdown
     	state[i] = THINKING;

	test(LEFT);
	test(RIGHT);
        // end of body
	if(next_count > 0)
	{
		sem_post(&next);
	}
	else
	{
		sem_post(&mutex);
	}
}

void initialization()
{
	int i;
	sem_init(&mutex,0,1);
	sem_init(&next,0,0);
	for(i = 0;i < N;i++)
	{
		state[i] = THINKING;
		sem_init(&x[i].sem,0,0);
		x[i].count = 0;
	}

}
