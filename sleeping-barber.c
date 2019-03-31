/* Pseudocode
    Initial values for semaphores :
    barberReady = 0
    waitingRoomSeatsMutex  = 1
    customerReady = 0

    Barber process :
        do {
            wait(customerReady)

            wait(waitingRoomSeatsMutex)

            num_chairs++

            signal(barberReady)
            signal(waitingRoomSeatsMutex)
        } while(true)

    Customer Process :
        do {
            wait(waitingRoomSeatsMutex)

            if numchairs > 0
                numchairs--
                signal(customerReady)
            signal(waitingRoomSeatsMutex)
        }
*/

#define MAX 20
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t waitingRoomSeatsMutex;
sem_t customerReady;
sem_t barberReady;
int num_chairs;
int customerwait;

void *barber(void *param) {
   int worktime;

   while(1) {
	  sem_wait(&customerReady);
	  sem_wait(&waitingRoomSeatsMutex);

	  num_chairs += 1;
	  printf("Barber: Taking a customer. Number of chairs available = %d\n",num_chairs);

	  sem_post(&barberReady);
	  sem_post(&waitingRoomSeatsMutex);

	  worktime = (rand() % 4) + 1;

	  printf("Barber: Cutting hair for %d seconds\n", worktime);
	  sleep(worktime);
    }
}

void *customer(void *param) {
   int waittime;

   while(1) {
	  sem_wait(&waitingRoomSeatsMutex);

	  if(num_chairs <= 0){
		   printf("Customer: Thread %u leaving with no haircut\n", (unsigned int)pthread_self());
		   sem_post(&waitingRoomSeatsMutex);
	  }
	  else{
		   num_chairs -= 1;
		   printf("Customer: Thread %u Sitting to wait. Number of chairs left = %d\n",(unsigned int)pthread_self(),num_chairs);

		   sem_post(&customerReady);

		   sem_post(&waitingRoomSeatsMutex);

		   sem_wait(&barberReady);

		   printf("Customer: Thread %u getting a haircut\n",(unsigned int)pthread_self());
	  }

	  waittime = (rand() % customerwait) + 1;

	  printf("Customer: Thread %u waiting %d seconds before attempting next haircut\n",(unsigned int)pthread_self(),waittime);
	  sleep(waittime);
     }
}

int main(int argc, char *argv[]) {
	pthread_t barberid;
	pthread_t customerid[MAX];

	printf("Main thread beginning\n");

   int runTime,customers,i;
   if (argc != 5){
	   printf("Please enter 4 arguments: <Program run time> <Number of customers>\n");
	   printf("<Number of chairs> <Customer wait time>\n");
	   exit(0);
   }
   runTime = atoi(argv[1]);
   customers = atoi(argv[2]);
   num_chairs = atoi(argv[3]);
   customerwait = atoi(argv[4]);

   sem_init(&waitingRoomSeatsMutex,0,1);
   sem_init(&customerReady,0,0);
   sem_init(&barberReady,0,0);

   pthread_create(&barberid, NULL, barber, NULL);
   printf("Creating barber thread with id %lu\n",barberid);

   for (i = 0; i < customers; i++){
	   pthread_create(&customerid[i], NULL, customer, NULL);
	   printf("Creating customer thread with id %lu\n",customerid[i]);
   }

   printf("Main thread sleeping for %d seconds\n", runTime);
   sleep(runTime);

   printf("Main thread exiting\n");
   exit(0);
}