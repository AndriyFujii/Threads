#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define SIZE 15
#define NBELT 3
#define MAXWEIGHT 5

// Initialize the mutex
pthread_mutex_t mutex;

int weight[SIZE];
int globalI;
int sum;
int nItems;

int updateLCD;

// Increments an item
void *incNItems(void *args)
{
	while(nItems < SIZE)
	{
		int activate = 1;
		pthread_mutex_lock(&mutex);
		printf("Input (1) to simulate a sensor activation: \n");
		scanf("%d", &activate);
		pthread_mutex_unlock(&mutex);
		
		if(activate == 1)
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
			
		}
		
		
	}
	updateLCD = 1;
	// Sums the total weight
	int partialSum = 0;
	int id;
	
	pthread_mutex_lock(&mutex);
	id = globalI;
	globalI++;
	pthread_mutex_unlock(&mutex);
	
	int startI = SIZE/NBELT*id;
	int endI = SIZE/NBELT*(id+1);
	
	for(int i = startI; i < endI; i++)
	{
		partialSum += weight[i];
	}
	
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
		updateLCD = 0;
		
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
			// Only updates if the value has changed
			if(nItems != oldValue)
			{
				printf("LCD nItems: %d\n", nItems);
				oldValue = nItems;
			}
			
		}
		printf("LCD nItems: %d\n", nItems);
		
		
		// Counting how long it takes to stop the threads and to sum the weight array
		clock_t clkBegin = clock();
		
		/* wait for the threads to finish */
		for(int i = 0; i < NBELT; i++)
		{
			if(pthread_join(thread[i], NULL))
			{
				fprintf(stderr, "Error joining thread\n");
				return 2;
			}
		}
		
		// Sum of the weight array
		int slowSum = 0;
		for(int i = 0; i < SIZE; i++)
		{
			slowSum += weight[i];
		}

		
		double clkDifference = (clock() - clkBegin)/(double)CLOCKS_PER_SEC;
		
		printf("Time: %f\n", clkDifference);
		printf("Total Weight (no thread): %d\n", slowSum);
		printf("Total Weight (thread): %d\n", sum);
		
		
		printf("Type (1) to retry: ");
		scanf("%d", &retry);
		printf("\n");
	}
	while(retry == 1);

	pthread_mutex_destroy(&mutex);
	
	return 0;
}
