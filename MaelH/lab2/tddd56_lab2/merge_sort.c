#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "merge_sort.h"
#if NB_THREADS > 0
#include <pthread.h>
#endif

#if NB_THREADS > 0
pthread_barrier_t thread_pool_barrier;
pthread_t thread[NB_THREADS];
struct mergesort_thread thread_data[NB_THREADS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void sequential_merge_sort(int *array, int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;

        sequential_merge_sort(array, left, mid);
        sequential_merge_sort(array, mid + 1, right);

        // Merge the sorted halves
        int n1 = mid - left + 1;
        int n2 = right - mid;

        int *L = (int *)malloc(n1 * sizeof(int));
        int *R = (int *)malloc(n2 * sizeof(int));

        for (int i = 0; i < n1; i++)
            L[i] = array[left + i];
        for (int j = 0; j < n2; j++)
            R[j] = array[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2)
        {
            if (L[i] <= R[j])
            {
                array[k] = L[i];
                i++;
            }
            else
            {
                array[k] = R[j];
                j++;
            }
            k++;
        }

        while (i < n1)
        {
            array[k] = L[i];
            i++;
            k++;
        }

        while (j < n2)
        {
            array[k] = R[j];
            j++;
            k++;
        }

        free(L);
        free(R);
    }
}

// Implementing heap sort algorithm to sort subarrays independantly, adapted from Geeks for Geeks Iterative heap sort

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void merge(int *arr, int left, int right) {
    int merged[right - left + 1];
    int m = (left + right) / 2;
    int i = left, j = m + 1, k = 0;
    while (i <= m && j <= right) {
        if (arr[i] <= arr[j]) merged[k] = arr[i++];
        else merged[k] = arr[j++];
        k++;
    }
    while (i <= m) merged[k++] = arr[i++];
    while (j <= right) merged[k++] = arr[j++];
    for (int p = 0; p < right - left + 1; p++) arr[p + left] = merged[p];
}

#if NB_THREADS > 0

pthread_barrier_t barrier;


void* simple_parallel_merge_sort(void * buffer)
{
	struct mergesort_thread *args = (struct mergesort_thread*) buffer;
    long int sub_size = args->array_size / NB_THREADS;
    long int left = args->id * sub_size;
    long int right = left + sub_size - 1;



    // Take the last elements of the arrays in the last thread if array size is not divisible by nb of threads
    if (args->array_size % NB_THREADS != 0 && args->id == NB_THREADS - 1) right = args->array_size - 1;



    // Sequential merge sorting of subarray
    sequential_merge_sort(args->array, left, right);

    

    // Wait for all subarrays to be sorted
    //printf("thread %d waiting at barrier\n", args->id);
    pthread_barrier_wait(&barrier);


    // Merging subarrays
    int k = 2;
    while(sub_size < args->array_size){
        //printf("thread %d\n", args->id);

        if (args->id % k == 0){
            //printf("merging\n");
            //ze last = sub size + array size % nb threads
            merge(args->array,left,left + (sub_size)*2-1);
            sub_size = sub_size*2;
            k = k*2;
            right = left + (sub_size)*2-1;
            
            //printf("loool\n");
        }else{
            sub_size = sub_size*2;
        }
        

        
        pthread_barrier_wait(&barrier);
    }



}
void* fully_parallel_merge_sort(void * buffer)
{
    struct mergesort_thread *args = (struct mergesort_thread*) buffer;
    /*
    * Add your code here
    */
}
#endif

void parallel_sort(int *array, int left, int right)
{
#if NB_THREADS > 0
    pthread_attr_t thread_attr;
    int i;

    // Initialize attributes
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);


    pthread_barrier_init(&barrier, NULL, NB_THREADS);

    // Create a thread pool
    for (i = 0; i < NB_THREADS; i++)
    {
        thread_data[i].id = i;
        thread_data[i].array = array;
        thread_data[i].array_size = right - left + 1;

        /*
        * Initialze other thread_data fields here
        */
#if ALGORITHM == 1
        pthread_create(&thread[i], &thread_attr, &simple_parallel_merge_sort, &thread_data[i]);
#endif
#if ALGORITHM == 2
        pthread_create(&thread[i], &thread_attr, &fully_parallel_merge_sort, &thread_data[i]);
#endif
    }
    for (i = 0; i < NB_THREADS; i++)
	{
        pthread_join(thread[i], NULL);
	}
#else
    printf("No threads available for parallel sort.\n");
#endif
}

void sort_array(int *array, int size)
{
#if ALGORITHM == 0
    sequential_merge_sort(array, 0, size - 1);
#else
    parallel_sort(array, 0, size - 1);
#endif
}