/* Pseudocode
Semaphores:
    lock = 1
    agent = 0

Agent process :
    do {
        wait(lock)
        randNum from 1..3
        if randNum == 1
            // put tobacco on table
            // put paper on table
            signal(smoker_match) // smoker with match
        if randNum == 2
            // put tobacco on table
            // put match on table
            signal(smoker_paper) // smoker with paper
        if randNum == 3
            // put match on table
            // put paper on table
            signal(smoker_tobacco) // smoker with tobacco
        signal(lock)
        wait(agent)
    } while(true)
Smoker Process :
    do {
        wait(smoker_tobacco)
        wait(lock)
        // pick up match
        // pick up paper
        wait(agent)
        wait(lock)
        // smoke
    }
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1

sem_t wish;
sem_t match_and_paper;
sem_t paper_and_tobacco;
sem_t tobacco_and_match;

void *pusher_agent(arg)
	void *arg;
{
	srand(time(NULL));

	while (TRUE) {
		sem_wait(&wish);

		int r = rand() % 3;

		switch (r) {
		case 0: sem_post(&match_and_paper); break;
		case 1: sem_post(&paper_and_tobacco); break;
		case 2: sem_post(&tobacco_and_match); break;
	}
	}
}

void *smoker_match(arg)
	void *arg;
{
	while (TRUE) {
		sem_wait(&paper_and_tobacco);
		printf("Smoker with match is smoking\n");
		sleep(1);
		sem_post(&wish);
	}
}

void *smoker_paper(arg)
	void *arg;
{
	while (TRUE) {
		sem_wait(&tobacco_and_match);
		printf("Smoker with paper is smoking\n");
		sleep(1);
		sem_post(&wish);
	}
}

void *smoker_tobacco(arg)
	void *arg;
{
	while (TRUE) {
		sem_wait(&match_and_paper);
		printf("Smoker with tobacco is smoking\n");
		sleep(1);
		sem_post(&wish);
	}
}

int main(void) {
	pthread_t agent, match, paper, tobacco;

	sem_init(&wish, 0, 1);
	sem_init(&match_and_paper, 0, 0);
	sem_init(&paper_and_tobacco, 0, 0);
	sem_init(&tobacco_and_match, 0, 0);

	pthread_create(&agent, NULL, pusher_agent, NULL);
	pthread_create(&match, NULL, smoker_match, NULL);
	pthread_create(&paper, NULL, smoker_paper, NULL);
	pthread_create(&tobacco, NULL, smoker_tobacco, NULL);

	pthread_join(tobacco, NULL);
	pthread_join(paper, NULL);
	pthread_join(match, NULL);
	pthread_join(agent, NULL);
}