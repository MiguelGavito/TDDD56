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

void buildMaxHeap(int arr[], int left, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[left + i] > arr[left + (i - 1) / 2]) {
            int j = i;
            while (j > 0 && arr[left + j] > arr[left + (j - 1) / 2]) {
                swap(&arr[left + j], &arr[left + (j - 1) / 2]);
                j = (j - 1) / 2;
            }
        }
    }
}

void heapSort(int arr[], int left, int right) {
    int n = right - left + 1;
    if (n <= 1) return;
    buildMaxHeap(arr, left, n);
    for (int i = n - 1; i > 0; i--) {
        swap(&arr[left], &arr[left + i]);
        int j = 0;
        int index;
        do {
            index = 2 * j + 1;
            if (index < i - 1 && arr[left + index] < arr[left + index + 1]) index++;
            if (index < i && arr[left + j] < arr[left + index]) {
                swap(&arr[left + j], &arr[left + index]);
            }
            j = index;
        } while (index < i);
    }
}

void* simple_parallel_merge_sort(void * buffer)
{
	struct mergesort_thread *args = (struct mergesort_thread*) buffer;
    /*
    * Add your code here
    */

}
void* fully_parallel_merge_sort(void * buffer)
{
    struct mergesort_thread *args = (struct mergesort_thread*) buffer;
    /*
    * Add your code here
    */
}

void parallel_sort(int *array, int left, int right)
{
#if NB_THREADS > 0
    pthread_attr_t thread_attr;
    int i;

    // Initialize attributes
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

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