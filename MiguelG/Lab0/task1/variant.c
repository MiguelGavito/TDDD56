
/* main.c */
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define N 1000

// This function computes a time span (double value)
// from two struct timespec pointers

double timediff(struct timespec *begin, struct timespec *end)
{
	double sec = 0.0, nsec = 0.0;
   if ((end->tv_nsec - begin->tv_nsec) < 0)
   {
      sec  = (double)(end->tv_sec  - begin->tv_sec  - 1);
      nsec = (double)(end->tv_nsec - begin->tv_nsec + 1000000000);
   } else
   {
      sec  = (double)(end->tv_sec  - begin->tv_sec );
      nsec = (double)(end->tv_nsec - begin->tv_nsec);
   }
   return sec + nsec / 1E9;
}


int main()
{
  int n;

  // scanf("%d",&n);
  n = N;
  // printf("%d",n);
  
  float A[n];

  int *address_i;
  
  clockid_t clk = CLOCK_REALTIME;

  struct timespec time1, time2;

  clock_gettime(clk, &time1);

  for (int i = 0; i < n; i++ ){
    A[i] = i*i+1;
    // printf("%f\n",A[i]);
    address_i=&i;
  }

  clock_gettime(clk, &time2);

  double elapse = timediff(&time1, &time2);

  

  for (int i = 0; i < n; i++ ){
    printf("A[%d]=%f\n",i ,A[i]);
  }

  printf("time[s] = %f\n", elapse);

  printf("\nAddresses:\n");
  printf("i:\t%p\n", (void *) address_i);
  printf("A:\t%p\nA[0]:\t%p\nA[1]:\t%p\nmain:\t%p\n",
         (void *)&A,
         (void *)&A[0],
         (void *)&A[1],
         (void *)&main
         );
  printf("\nSizes\ni:%zu\nA:%zu\nA[0]:%zu\n", sizeof(*address_i), sizeof(A), sizeof(A[0]));


  // printf("\nout of bound\nA[n]:%f\n",A[n]);
  // A[n] = 5;
  // printf("Writed 5 on A[n]\nA[n]:%f\n",A[n]);
  
  float *px = &A[n-1];

  printf("\nA[1] conversion as int: %d\n",(int)A[n-1]);
  printf("\nA[1] reinterpret as int: %d\n",*((int *)px));

  return 0;
  
}
