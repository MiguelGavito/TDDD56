/*task3.c*/
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define NUM_THREADS 10 // threads 
#define SIZE 100000000

float *A; // Array init

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

typedef struct{
  int start;
  int end;
  double elapsed;
  double sum ;
} ThreadData;

void *calculate(void* arg){
  ThreadData *data = (ThreadData*)arg;
  
  double local_sum = 0.0;

  struct timespec time1, time2;
  clock_gettime(CLOCK_MONOTONIC,&time1);


  for (int i = data->start; i < data->end; i++){
        A[i] = (float)i * (float)i + 1.0f;
        local_sum += A[i];  
  }

  clock_gettime(CLOCK_MONOTONIC,&time2);

  data->sum = local_sum;
  data->elapsed = timediff(&time1,&time2);

  return NULL;
  
} 

int main(int argc, char *argv[])
{

  // int *address_i;
  
  clockid_t clk = CLOCK_MONOTONIC;

  struct timespec time1, time2;


  // 1. Read in an integer number into a variable n
  int n = SIZE;
  // scanf("%d",&n);

  // 2. Dynamically allocate an array A of n floatingpoint elements.
  A = malloc(n * sizeof(float));

  // Create Multiple parallel pthreads, p in total
  pthread_t *threads = malloc(NUM_THREADS * sizeof(pthread_t));
  ThreadData *data = malloc(NUM_THREADS * sizeof(ThreadData));

  int elements_per_thread = n / NUM_THREADS;

  float sum = 0;

  clock_gettime(clk, &time1);

  //creating pthreaths
  for (int i = 0; i < NUM_THREADS; i++){
    struct timespec time10, time20;

    
    data[i].start = i * elements_per_thread;
    data[i].end = (i == NUM_THREADS - 1)
                  ? n
                  : (i+1) * elements_per_thread;


    pthread_create(
      &(threads[i]),
      NULL,
      calculate,
      &data[i]
    );

  }


  // wait until the thread end
  for (int i = 0; i < NUM_THREADS; i++){
    
    pthread_join(threads[i],NULL);
    
    sum += data[i].sum;
  }

  clock_gettime(clk, &time2);

  double elapse = timediff(&time1, &time2);

  

  for (int i = 0; i < n; i++ ){
    // if (i < 5 || i > n-4){
      // printf("A[%d]=%f\n",i ,A[i]);
    // }
  }

  printf("time[s] = %f\n", elapse);

  printf("sum = %f\n", sum);

  free(A);
  free(threads);
  free(data);

  return 0;
  
}
