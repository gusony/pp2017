#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int isprime(int n) {
  int i,squareroot;
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

int main(int argc, char *argv[])
{
  long long int pc,       /* prime counter */
                foundone; /* most recent prime found */
  long long int n, limit;
  time_t start_time, end_time;
  
  sscanf(argv[1],"%llu",&limit);	
  printf("Starting. Numbers to be scanned= %lld\n",limit);

  pc=4;     /* Assume (2,3,5,7) are counted here */
  start_time = clock();
  MPI_Init(&argc, &argv);
  for (n=11; n<=limit; n=n+2) {
    if (isprime(n)) {
      pc++;
      foundone = n;
    }			
  }
  MPI_Finalize();
  end_time = clock();
  printf("use %fs\n",(end_time - start_time) / CLOCKS_PER_SEC );
  printf("Done. Largest prime is %d Total primes %d\n",foundone,pc);

  return 0;
} 
