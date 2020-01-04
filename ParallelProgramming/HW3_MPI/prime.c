#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mpi.h"

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
  long long int pc; /* prime counter */
  long long int foundone; /* most recent prime found */
  long long int n, limit;

  int proc_num, my_rank;
  long long int my_pc, my_foundone;
  
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  sscanf(argv[1],"%llu",&limit); 

  my_pc = 0;
  my_foundone = 7;
  for (n=11+2*my_rank; n<=limit; n+=2*proc_num){
    if (isprime(n)) {
      my_pc++;
      my_foundone = n;
    }     
  }

  MPI_Reduce(&my_pc, &pc, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Reduce(&my_foundone, &foundone, 1, MPI_LONG_LONG_INT, MPI_MAX, 0, MPI_COMM_WORLD);

  if(my_rank==0){ 
    printf("Starting. Numbers to be scanned= %lld\n", limit);
    pc += 4; /* 2,3,5,7 */
    printf("Done. Largest prime is %d Total primes %d\n", foundone, pc);
  }

  MPI_Finalize();

  return 0;
} 