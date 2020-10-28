#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#define SIZE 1500
#define NBELT 3
#define MAXWEIGHT 5

// Initialize the mutex
pthread_mutex_t mutex;

int weight[SIZE];
int globalI;
int sum;
int nItems;

struct timeval tic;
struct timeval ticTotal;
struct timeval toc;

double totalTime;
double sumBeltUpdateTimes;
double sumPartialSumTimes;
double sumLCDTimes;
int nLCDTimes;

// Increments an item
void *incNItems(void *args)
{
	gettimeofday(&tic, 0);
	while(nItems < SIZE)
	{
		
		// Making sure it's not going over
		
		if(nItems < SIZE)
		{
			// Mutex lock the shared variables
			pthread_mutex_lock(&mutex);

			// Random weight from 1 - MAXWEIGHT
			weight[nItems] = rand() % MAXWEIGHT + 1;
			nItems += 1;

			//Unlock the shared variables
			pthread_mutex_unlock(&mutex);

			//printf("thread %lu nItem %d\n", pthread_self(), nItems);
		}

		usleep(1000);
		
	}
	gettimeofday(&toc, 0);
	sumBeltUpdateTimes += (toc.tv_sec-tic.tv_sec)*1000000 + toc.tv_usec-tic.tv_usec;

	// Sums the total weight
	int partialSum = 0;
	int id;
	
	pthread_mutex_lock(&mutex);
	id = globalI;
	globalI++;
	pthread_mutex_unlock(&mutex);
	
	int startI = SIZE/NBELT*id;
	int endI = SIZE/NBELT*(id+1);
	
	gettimeofday(&tic, 0);
	for(int i = startI; i < endI; i++)
	{
		partialSum += weight[i];
	}
	gettimeofday(&toc, 0);
	sumPartialSumTimes += (toc.tv_sec-tic.tv_sec)*1000000 + toc.tv_usec-tic.tv_usec;;
	
	pthread_mutex_lock(&mutex);
	sum += partialSum;
	pthread_mutex_unlock(&mutex);
	
	return NULL;
}


int main()
{
	srand(time(NULL));
	

	pthread_t thread[NBELT];
	
	int retry;
	
	do
	{
		//Resetting values
		nItems = 0;
		globalI = 0;
		retry = 0;
		
		gettimeofday(&ticTotal, 0);
		sumBeltUpdateTimes = 0;
		sumPartialSumTimes = 0;
		sumLCDTimes = 0;
		nLCDTimes = 0;
		
		if(pthread_mutex_init(&mutex, NULL))
		{
			fprintf(stderr, "Error creating mutex\n");
			return 1;
		}
		
		//Creates the threads
		for(int i = 0; i < NBELT; i++)
		{
			if(pthread_create(&thread[i], NULL, incNItems, NULL))
			{
				fprintf(stderr, "Error creating thread\n");
				return 1;
			}
		}
		
		int oldValue = -1;
		// Update LCD display
		while(nItems < SIZE)
		{
			gettimeofday(&tic, 0);
			// Only updates if the value has changed
			if(nItems != oldValue)
			{
				printf("LCD nItems: %d\n", nItems);
				oldValue = nItems;
				gettimeofday(&toc, 0);
				sumLCDTimes += (toc.tv_sec-tic.tv_sec)*1000000 + toc.tv_usec-tic.tv_usec;
				nLCDTimes++;
			}
			
		}
		printf("LCD nItems: %d\n", nItems);
		
		
		/* wait for the threads to finish */
		for(int i = 0; i < NBELT; i++)
		{
			if(pthread_join(thread[i], NULL))
			{
				fprintf(stderr, "Error joining thread\n");
				return 2;
			}
		}
		gettimeofday(&toc, 0);
		totalTime = (toc.tv_sec-ticTotal.tv_sec)*1000000 + toc.tv_usec-ticTotal.tv_usec;
		

		printf("Total Weight: %d\n", sum);
		printf("Total belt update time: %f\n", sumBeltUpdateTimes/1000000);
		printf("Avg LCD update time: %f\n", sumLCDTimes/(double)nLCDTimes/1000000);
		printf("Total partial sum time: %f\n", sumPartialSumTimes/1000000);
		printf("Total time: %f\n", totalTime/1000000);
		
		printf("Type (1) to retry: ");
		scanf("%d", &retry);
		printf("\n");
	}
	while(retry == 1);

	pthread_mutex_destroy(&mutex);
	
	return 0;
}
