#include <stdio.h>
#include <omp.h>
#include <unistd.h>

#define SIZE 15
#define NBELT 3
#define MAXWEIGHT 5

int main(int argc, char** argv)
{
	srand(time(NULL));
	
	int nItems = 0;
	int weight[SIZE];
	int sumThread = 0;
	int retry;
	do
	{
		// Reseting values
		nItems = 0;
		
    	#pragma omp parallel shared(nItems, weight) num_threads(4)
    	{
    	
    		int oldValue = -1;
			// Single so one task is always updating the LCD
			// Nowait because other tasks can happen while the LCD updates
			#pragma omp single nowait
			// Update LCD display
			{
				while(nItems < SIZE)
				{
					// Only updates if the value has changed
					if(nItems != oldValue)
					{
						printf("LCD nItems: %d\n", nItems);
						oldValue = nItems;
					}
				}
			}
			
			while(nItems < SIZE)
			{

				// Making sure it's not going over
				if(nItems < SIZE)
				{
					// Critical so only 1 task updates the values
					#pragma omp critical
					{
						// Random weight from 1 - MAXWEIGHT
						weight[nItems] = rand() % MAXWEIGHT + 1;
						nItems += 1;
					}
					printf("thread %u nItem %d\n", omp_get_thread_num(), nItems);
				}
				usleep(100000);
			}
		}
		
		// Parallel sum of the weight array
		#pragma omp parallel for reduction(+:sumThread) num_threads(4)
		for(int i = 0; i < SIZE; i++)
		{
			sumThread+=weight[i];
		}
		
		int sum = 0;
		for(int i = 0; i < SIZE; i++)
		{
			sum += weight[i];
		}
		
		printf("Number of items: %d\n", nItems);
		printf("Total Weight (Array): %d\n", sum);
		printf("Total Weight (Thread): %d\n", sumThread);
		
		printf("Type (1) to retry: ");
		scanf("%d", &retry);
		printf("\n");
	}
	while(retry == 1);
	
    return 0;
}
