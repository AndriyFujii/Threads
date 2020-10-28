#include <stdio.h>
#include <omp.h>
#include <sys/time.h>
#include <unistd.h>

#define SIZE 1500
#define NBELT 3
#define MAXWEIGHT 5

int main(int argc, char** argv)
{
	srand(time(NULL));
	
	int nItems = 0;
	int weight[SIZE];
	int sumThread = 0;
	int retry;
	
	struct timeval tic;
	struct timeval ticTotal;
	struct timeval toc;
	
	double totalTime;
	double sumBeltUpdateTimes;
	double sumPartialSumTimes;
	double sumLCDTimes;
	int nLCDTimes;
	do
	{
		// Reseting values
		nItems = 0;
		
		gettimeofday(&ticTotal, 0);
		
		sumBeltUpdateTimes = 0;
		sumPartialSumTimes = 0;
		sumLCDTimes = 0;
		nLCDTimes = 0;
		
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
			}
			
			gettimeofday(&tic, 0);
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
					//printf("thread %u nItem %d\n", omp_get_thread_num(), nItems);
				}
				usleep(1000);
			}
			gettimeofday(&toc, 0);
			sumBeltUpdateTimes += (toc.tv_sec-tic.tv_sec)*1000000 + toc.tv_usec-tic.tv_usec;
		}
		
		
		gettimeofday(&tic, 0);
		// Parallel sum of the weight array
		#pragma omp parallel for reduction(+:sumThread) num_threads(4)
		for(int i = 0; i < SIZE; i++)
		{
			sumThread+=weight[i];
		}
		gettimeofday(&toc, 0);
		sumPartialSumTimes += (toc.tv_sec-tic.tv_sec)*1000000 + toc.tv_usec-tic.tv_usec;
		totalTime = (toc.tv_sec-ticTotal.tv_sec)*1000000 + toc.tv_usec-ticTotal.tv_usec;
		
		
		printf("Number of items: %d\n", nItems);
		printf("Total Weight: %d\n", sumThread);
		printf("Total belt update time: %f\n", sumBeltUpdateTimes/1000000);
		printf("Avg LCD update time: %f\n", sumLCDTimes/(double)nLCDTimes/1000000);
		printf("Total partial sum time: %f\n", sumPartialSumTimes/1000000);
		printf("Total time: %f\n", totalTime/1000000);
		
		printf("Type (1) to retry: ");
		scanf("%d", &retry);
		printf("\n");
	}
	while(retry == 1);
	
    return 0;
}
