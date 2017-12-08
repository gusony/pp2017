#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

#define PI 3.1415926535

int main(int argc, char **argv)
{
  long long i, num_intervals, avg;
  long long up_bound, low_bound;
  double rect_width, area, sum, x_middle,temp_sum;
  int size = -1, rank = -1;
  MPI_Status status;

  sscanf(argv[1],"%llu",&num_intervals);
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);


  rect_width = PI / num_intervals;
  avg = (long long int)num_intervals / size;

  up_bound = avg * (rank+1);
  low_bound = (avg * rank)+1;
  sum = 0;
  for(i = low_bound; i < up_bound + 1; i++) {

    /* find the middle of the interval on the X-axis. */

    x_middle = (i - 0.5) * rect_width;
    area = sin(x_middle) * rect_width;
    sum = sum + area;
  }
  if(rank == 0){
    for(int i=1; i<size; i++){
      MPI_Recv(&temp_sum, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
      sum += temp_sum;
    }
    printf("The total area is: %f\n", (float)sum);
  }
  else {
    MPI_Send(&sum, 1, MPI_DOUBLE,0, 0, MPI_COMM_WORLD);
  }
  MPI_Finalize();


  return 0;
}
