#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int isprime(long long int n) {
  long long int i,squareroot;
  if (n>10) {
    squareroot = (int) sqrt(n);
    for (i=3; i<=squareroot; i=i+2)
      if ((n%i)==0)
        return 0;
    return 1;
  }
  else
    return 0;
}

int main(int argc, char *argv[]){
	long long int pc,       /* prime counter */
                  foundone; /* most recent prime found */
	long long int n, limit,avg;
	time_t start_time, end_time;
	int size=-1,rank=-1;

	sscanf(argv[1],"%llu",&limit);
	printf("Starting. Numbers to be scanned= %lld\n",limit);

	pc=4;     /* Assume (2,3,5,7) are counted here */
	start_time = clock();
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	avg = (long long int)(limit/size);

	if(rank == size-1){//least process
	    for (n = (avg*rank)+1; n <= limit; n=n+2){
	        if (isprime(n)) {
	            pc++;
	            foundone = n;
	        }
	    }
	}
	else{
		for (n = avg*rank+1 ; n <= avg*(rank+1); n=n+2){
			if (isprime(n)) {
				pc++;
				foundone = n;
			}
		}
	}

	MPI_Finalize();
	end_time = clock();
	printf("rank%d, use %.6fs, ",rank,(end_time - start_time)/(float)CLOCKS_PER_SEC);
	printf("Done. Largest prime is %d Total primes %d\n",foundone,pc);

	return 0;
}

