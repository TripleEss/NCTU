#include <stdio.h>
#include <math.h>

#include "mpi.h"

#define PI 3.1415926535

int main(int argc, char **argv) 
{
  long long int i, num_intervals;
  double rect_width, area, sum, x_middle;

  int proc_num, my_rank;
  long long int my_num_intervals;
  double my_sum;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  sscanf(argv[1],"%llu",&num_intervals);
  rect_width = PI/num_intervals;

  my_sum = 0;
  for(i=1+my_rank; i<num_intervals+1; i+=proc_num){
    x_middle = (i-0.5)*rect_width; /* find the middle of the interval on the X-axis */ 
    area = sin(x_middle)*rect_width; 
    my_sum += area;
  }

  MPI_Reduce(&my_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if(my_rank==0){
    printf("The total area is: %f\n", (float)sum);
  }

  MPI_Finalize();

  return 0;
}   