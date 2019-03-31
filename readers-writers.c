/* Pseudocode
    Initial Values of semaphores:
    mutex = 1 // used for maintain access for `reader`
    writeaccess = 1

    Readers' process :
        do {
            wait(mutex)

            reader++

            if (reader == 1)
                wait(writeaccess)

            signal(mutex)

            // reader process is reading.....

            wait(mutex)

            reader--

            if (reader == 0)
                signal(writeaccess)

            signal(mutex)
        } while(true)

    Writers' process :
        do {
            wait(writeaccess)

            // performs the write

            signal(writeaccess)
        } while(true)
*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 5

sem_t mutex;
sem_t writeaccess;

int data = 0;
int rcount = 0;

void *reader(arg)
	void *arg;
{
	sem_wait(&mutex);

	++rcount;

	if (rcount == 1) {
		sem_wait(&writeaccess);
	}

	sem_post(&mutex);
	printf("Data read by the reader %d is %d\n", (int)(__intptr_t) arg, data);
	sleep(1);
	sem_wait(&mutex);

	--rcount;

	if (rcount == 0) {
		sem_post(&writeaccess);
	}

	sem_post(&mutex);
}

void *writer(arg)
	void *arg;
{
	sem_wait(&writeaccess);

	++data;

	printf("Data written by the writer %d is %d\n", (int)(__intptr_t) arg, data);
	sleep(1);
	sem_post(&writeaccess);
}

int main(void) {
	int b;
	pthread_t rtid[N], wtid[N];

	sem_init(&mutex, 0, 1);
	sem_init(&writeaccess, 0, 1);

	for (int i = 0; i < N; ++i) {
		pthread_create(&wtid[i], NULL, writer, (void *)(__intptr_t) i);
		pthread_create(&rtid[i], NULL, reader, (void *)(__intptr_t) i);
	}
	for (int i = 0; i < N; ++i) {
		pthread_join(wtid[i], NULL);
		pthread_join(rtid[i], NULL);
	}
}

/* sample output:
Data written by the writer 0 is 1
Data read by the reader 0 is 1
Data read by the reader 1 is 1
Data read by the reader 3 is 1
Data read by the reader 2 is 1
Data read by the reader 4 is 1
Data written by the writer 1 is 2
Data written by the writer 2 is 3
Data written by the writer 4 is 4
Data written by the writer 3 is 5
*/