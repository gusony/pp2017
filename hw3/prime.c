#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

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
	long long int pc,       // prime counter
                  foundone; // most recent prime found
	long long int temp_pc,       // to receive data from other thread
                  temp_foundone; // to receive data from other thread
    long long int n, limit,avg,up_bound, recv_buf;
	time_t start_time, end_time;
	int size=-1,rank=-1;

	sscanf(argv[1],"%llu",&limit);
	if(rank == size-1)
		printf("Starting. Numbers to be scanned= %lld\n",limit);
	pc=4;     /* Assume (2,3,5,7) are counted here */
	start_time = clock();
	int tag = 0;
	char *message = malloc(sizeof(char)*100);

    MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	avg = (long long int)(limit/size);
	if(rank == size-1)   //least process
		up_bound = limit;
	else
		up_bound = avg*(rank+1);


	if((avg*rank+1)%2 == 0)//start number must be odd number
		n = avg*rank+2;
	else
	    n = avg*rank+1;

	for (n ; n<=up_bound; n=n+2){
        if (isprime(n)) {
            pc++;
            foundone = n;
        }
    }


    if(rank == size-1){
    	for(int i=0; i<rank; i++){
    		MPI_Recv(&recv_buf, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);

    		pc += recv_buf;
    	}
    	end_time = clock();
		//printf("use %.6fs, ",(end_time - start_time)/(float)CLOCKS_PER_SEC);
		printf("Done. Largest prime is %d Total primes %d\n",foundone,pc);
	}
	else{
		MPI_Send(&pc, 1, MPI_INT, size-1, tag, MPI_COMM_WORLD);
	}

    MPI_Finalize();



	return 0;
}

