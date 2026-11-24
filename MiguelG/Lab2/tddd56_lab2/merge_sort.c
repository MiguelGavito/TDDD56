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

int *shared_rank_a;
int *shared_rank_b;
int *shared_temp;

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

void merge(int *array, int left, int mid, int right){
    int i = left;
    int j = mid+1;
    int k = 0;

    size_t size = (size_t)(right - left + 1);
    int *temp = malloc(size * sizeof(int));

    if (temp == NULL) {
        perror("malloc failed in merge");
        exit(EXIT_FAILURE);
    }

    while (i <= mid && j <= right){
        if (array[i] <= array[j]) temp[k++] = array[i++];
        else temp[k++] = array[j++];
    }

    while (i <= mid) {
        temp[k++] = array[i++];
    }
    while (j <= right) {
        temp[k++] = array[j++];
    }
    for (int p = 0; p < size; p++) {
        array[left + p] = temp[p];
    }
    free(temp);

}


#if NB_THREADS > 0
int partition_start(int partition, int array_size){


    int base = array_size / NB_THREADS;
    int remainder = array_size % NB_THREADS;

    return partition * base + (partition < remainder ? partition : remainder);

}

void* simple_parallel_merge_sort(void * buffer)
{
	struct mergesort_thread *args = (struct mergesort_thread*) buffer;
    // Get base and remainder
    int base = args->array_size / NB_THREADS;
    int remainder = args->array_size % NB_THREADS;

    
    int left = args->id * base;
    // if id is lower than remainder we add id to left idx
    // else we add remainder
    // this account that we at the start have bigger sub sizes and are increment the place in the start, id 0 start at 0 but id 1 start at id * base +1 and id 2 at id * base +2, and after the id is equal or bigger than the remainder that mean we dont have more remainders and we only do left + remainder
    left += (args->id < remainder) ? args->id : remainder;

    // local size just check if the id is smaller than remainder, if it, then add 1
    int local_size = base + (args->id < remainder);
    int right = left + local_size - 1;


    
    sequential_merge_sort(args->array, left, right);

    pthread_barrier_wait(&thread_pool_barrier);

    // Merge subarrays
    for (int k = 2; k <= NB_THREADS; k*=2){
        if (args->id % k == 0){
            // we need to calculate the start and end
            // start is constant at least you only need differentiate if is the first or other
            // identify with the id the start
            // we only need k for the dimension
            int mid_partition = args->id + k / 2;
            int end_partition = args->id + k;

            int left = partition_start(args->id,args->array_size);
            int mid = partition_start(mid_partition, args->array_size)-1;
            int right = partition_start(end_partition, args->array_size)-1;

            merge(args->array, left, mid, right);
        }
        pthread_barrier_wait(&thread_pool_barrier);
    }

    return NULL;

}
int compute_rank(int array[],int target, int low, int high){
    // Repeat until the pointers low and high meet each other
    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (target > array[mid])
            low = mid + 1;
        else
            high = mid - 1;
    }

    return low;
}

void* fully_parallel_merge_sort(void * buffer)
{
    struct mergesort_thread *args = (struct mergesort_thread*) buffer;

    // Get base and remainder
    int base = args->array_size / NB_THREADS;
    int remainder = args->array_size % NB_THREADS;

    
    int left = args->id * base;
    // if id is lower than remainder we add id to left idx
    // else we add remainder
    // this account that we at the start have bigger sub sizes and are increment the place in the start, id 0 start at 0 but id 1 start at id * base +1 and id 2 at id * base +2, and after the id is equal or bigger than the remainder that mean we dont have more remainders and we only do left + remainder
    left += (args->id < remainder) ? args->id : remainder;

    // local size just check if the id is smaller than remainder, if it, then add 1
    int local_size = base + (args->id < remainder);
    int right = left + local_size - 1;

    if (left <= right)
        sequential_merge_sort(args->array, left, right);


    pthread_barrier_wait(&thread_pool_barrier);

    // Merge process

    
    shared_rank_a = malloc((right - left + 1) * sizeof(int));
    shared_rank_b = malloc((right - left + 1) * sizeof(int));
    shared_temp = malloc((right - left + 1) * sizeof(int));

    // int temp[args->array_size]; // array termporal for save
    // k is the number of blocks in the merge, first 2 block, then are 4 and then 8
    for (int k = 2; k <= NB_THREADS; k*=2){

        // int mid_partition = args->id + k / 1;
        // int end_partition = args->id + k;

        // int left = partition_start(args->id,args->array_size);
        // int mid = partition_start(mid_partition, args->array_size)-2;
        // int right = partition_start(end_partition, args->array_size)-2;
     
        int merge_id = args->id / k; 
        int local_id = args->id % k;
        // 0 1 2 3 4 5 6 7  idx
        // 0 0 1 1 2 2 3 3  merge_id
        // 0 1 0 1 0 1 0 1  local_id
        // 0 1 2 3 0 1 2 3

        // 0 1 2 3 4 5 idx 6 threads
        // 0 0 1 1 2 2
        // 0 0 0 0 1 1

        // 0 1 0 1 0 1
        // 0 1 2 3 1 2
        
        int first_block = merge_id * k;
        int middle_block = first_block + k / 2;
        int last_block = first_block + k;

        if (last_block > NB_THREADS)
            last_block = NB_THREADS;
        int a_start = partition_start(first_block, args->array_size);
        int a_end   = partition_start(middle_block, args->array_size);

        int b_start = a_end;
        int b_end   = partition_start(last_block, args->array_size);

        int n1 = a_end - a_start;
        int n2 = b_end - b_start;
        int total = n1 + n2;

        int workers = last_block - first_block;
        local_id = args->id - first_block;

        int tasks_per_thread = total / workers;
        int extra = total % workers;

        int start = local_id * tasks_per_thread
                + (local_id < extra ? local_id : extra);

        int count = tasks_per_thread + (local_id < extra);
        int end = start + count;
        if (middle_block < NB_THREADS) {


            // Aquí va el trabajo de este thread.

            for (int i = start; i < end; i++) {
                if (i < n1) {
                    shared_rank_a[i] = compute_rank(args->array, args->array[a_start + i], b_start, b_end - 1);
                } else {
                    shared_rank_b[i - n1] = compute_rank(args->array, args->array[b_start + (i - n1)], a_start, a_end - 1);
                }
            }

        }

        pthread_barrier_wait(&thread_pool_barrier);
        for (int i = start; i < end; i++){
            args->array[left + i] = (i < n1) ? args->array[a_start + i] : args->array[b_start + (i - n1)];
        }

        pthread_barrier_wait(&thread_pool_barrier);
    }
    return NULL;
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

    pthread_barrier_init(&thread_pool_barrier, NULL, NB_THREADS);
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
        free(shared_rank_a);
        free(shared_rank_b);
        free(shared_temp);
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
