#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#define SIZE 1500
#define NBELT 3
#define MAXWEIGHT 5


int main()
{
	srand(time(NULL));
	
	struct timeval tic;
	struct timeval ticTotal;
	struct timeval toc;
	
	int nItems;
	int weight[SIZE];
	int retry;
	
	double totalTime;
	double sumBeltLCDTimes;

	double sumTimes;

	int nLCDTimes;
	
	do
	{
		
		//Resetting values
		nItems = 0;
		
		gettimeofday(&ticTotal, 0);
		
		

		sumBeltLCDTimes = 0;
		sumTimes = 0;

		nLCDTimes = 0;
		
		gettimeofday(&tic, 0);
		while(nItems < SIZE)
		{
			// Making sure it's not going over
			if(nItems < SIZE)
			{
				
				printf("LCD nItems: %d\n", nItems);
				
				// Random weight from 1 - MAXWEIGHT
				weight[nItems] = rand() % MAXWEIGHT + 1;
				nItems += 1;
				
				//printf("nItem %d\n", nItems);
			}
			usleep(1000);
			
		}
		printf("LCD nItems: %d\n", nItems);
		gettimeofday(&toc, 0);
		sumBeltLCDTimes = (toc.tv_sec-tic.tv_sec)*1000000 + toc.tv_usec-tic.tv_usec;
		
		
		int sum = 0;
		gettimeofday(&tic, 0);
		for(int i = 0; i < SIZE; i++)
		{
			sum += weight[i];
		}
		gettimeofday(&toc, 0);
		sumTimes = (toc.tv_sec-tic.tv_sec)*1000000 + toc.tv_usec-tic.tv_usec;
		totalTime = (toc.tv_sec-ticTotal.tv_sec)*1000000 + toc.tv_usec-ticTotal.tv_usec;
		
		printf("Total Weight: %d\n", sum);
		printf("LCD and belt total update time: %f\n", sumBeltLCDTimes/1000000);
		printf("Total sum time: %f\n", sumTimes/1000000);
		printf("Total time: %f\n", totalTime/1000000);
		
		printf("Type (1) to retry: ");
		scanf("%d", &retry);
		printf("\n");
	}
	while(retry == 1);

	
	return 0;
}
