#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define SIZE 30
#define NBELT 3
#define MAXWEIGHT 5

// Initialize the mutex
pthread_mutex_t mutex;

int weight[SIZE];
int globalI;
int sum;
int nItems;

double sumBeltUpdateTimes;
int nBeltUpdateTimes;
double sumPartialSumTimes;
double sumLCDTimes;
int nLCDTimes;

// Increments an item
void *incNItems(void *args)
{
	clock_t clkBegin;
	while(nItems < SIZE)
	{

		// Making sure it's not going over
		
		if(nItems < SIZE)
		{
			// Mutex lock the shared variables
			pthread_mutex_lock(&mutex);
			clkBegin = clock();
			
			// Random weight from 1 - MAXWEIGHT
			weight[nItems] = rand() % MAXWEIGHT + 1;
			nItems += 1;
			
			sumBeltUpdateTimes += (clock() - clkBegin)/(double)CLOCKS_PER_SEC;
			nBeltUpdateTimes++;
			//printf("Belt update time: %f\n", clkDifference);
			
			
			//Unlock the shared variables
			pthread_mutex_unlock(&mutex);
			
			
			//printf("thread %lu nItem %d\n", pthread_self(), nItems);
		}
		usleep(100000);
		
	}
	// Sums the total weight
	int partialSum = 0;
	int id;
	
	pthread_mutex_lock(&mutex);
	id = globalI;
	globalI++;
	pthread_mutex_unlock(&mutex);
	
	int startI = SIZE/NBELT*id;
	int endI = SIZE/NBELT*(id+1);
	
	clkBegin = clock();
	
	for(int i = startI; i < endI; i++)
	{
		partialSum += weight[i];
	}
	
	sumPartialSumTimes += (clock() - clkBegin)/(double)CLOCKS_PER_SEC;
	
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
		
		sumBeltUpdateTimes = 0;
		nBeltUpdateTimes = 0;
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
			clock_t clkBegin = clock();
			// Only updates if the value has changed
			if(nItems != oldValue)
			{
				printf("LCD nItems: %d\n", nItems);
				oldValue = nItems;
				sumLCDTimes += (clock() - clkBegin)/(double)CLOCKS_PER_SEC;
				nLCDTimes++;
			}
			
		}
		
		
		// Counting how long it takes to stop the threads and to sum the weight array
		//clock_t clkBegin = clock();
		
		/* wait for the threads to finish */
		for(int i = 0; i < NBELT; i++)
		{
			if(pthread_join(thread[i], NULL))
			{
				fprintf(stderr, "Error joining thread\n");
				return 2;
			}
		}
		

		printf("Total Weight (thread): %d\n", sum);
		printf("Avg belt update time: %f\n", sumBeltUpdateTimes/(double)nBeltUpdateTimes);
		printf("Avg LCD update time: %f\n", sumLCDTimes/(double)nLCDTimes);
		printf("Total partial sum time: %f\n", sumPartialSumTimes);
		
		printf("Type (1) to retry: ");
		scanf("%d", &retry);
		printf("\n");
	}
	while(retry == 1);

	pthread_mutex_destroy(&mutex);
	
	return 0;
}
