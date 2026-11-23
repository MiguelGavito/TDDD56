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

void merge(int *array, int left, int right) {
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

/*
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
    free(merged);
}
    */

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
/*
 * ===================== Fully parallel merge sort =====================
 *
 * Following lecture 7 ("Fully Parallel Mergesort"):
 *  - Phase 1: each thread sequentially sorts its own slice of the array.
 *  - Phase 2: bottom-up iterative merging. At EVERY level, ALL threads
 *    share the merging work: the output of every merge is split into
 *    NB_THREADS contiguous segments, and each thread merges only the
 *    part of each merge that falls inside its own segment.
 *    The slice boundaries inside a merge are found by cross-ranking
 *    (binary searches), cf. ParMerge in the lecture.
 *  - Double buffering: level i reads from src and writes to dst, then
 *    the buffers are swapped. This is necessary because the output
 *    range of a merge equals its input range, so writing in place
 *    would destroy elements that other threads still have to read.
 */

static int *fully_par_aux = NULL; /* auxiliary buffer, allocated once */

/* Merge the sorted runs L = src[a0..a1] and R = src[b0..b1] into dst,
 * but only write the output positions in [out_lo..out_hi].
 * The split point of the slice is found with binary searches, so each
 * thread only does O(slice length + log) work per merge. Ties are
 * always resolved in favour of the left run (stable), which makes the
 * split points computed independently by different threads consistent. */
static void merge_slice(const int *src, int *dst,
                        int a0, int a1, int b0, int b1,
                        int out_lo, int out_hi)
{
    int lenL = a1 - a0 + 1;
    int lenR = b1 - b0 + 1;
    int o = out_lo - a0;        /* offset inside the merged output */

    /* Smallest i such that taking i elements from L and o-i from R is a
     * valid merge partition of the first o outputs:
     *   i = number of left elements among the first o outputs. */
    int lo_i = o - lenR; if (lo_i < 0) lo_i = 0;
    int hi_i = (o < lenL) ? o : lenL;
    while (lo_i < hi_i) {
        int mid = (lo_i + hi_i) / 2;
        int j = o - mid; /* > 0 here, and j < lenR since mid > o - lenR */
        if (mid == lenL || src[b0 + j - 1] <= src[a0 + mid])
            hi_i = mid;
        else
            lo_i = mid + 1;
    }
    int i0 = lo_i;
    int j0 = o - i0;

    /* Standard 2-to-1 merge of the slice, in O(slice) steps */
    int i = i0, j = j0, pos = out_lo;
    while (pos <= out_hi) {
        int take_left;
        if (i == lenL)      take_left = 0;
        else if (j == lenR) take_left = 1;
        else                take_left = (src[a0 + i] <= src[b0 + j]);
        if (take_left)
            dst[pos++] = src[a0 + i++];
        else
            dst[pos++] = src[b0 + j++];
    }
}

void* fully_parallel_merge_sort(void * buffer)
{
    struct mergesort_thread *args = (struct mergesort_thread*) buffer;
    int n = args->array_size;
    int p = NB_THREADS;

    /* Initial run length = length of one thread's slice */
    int w = (n + p - 1) / p;
    if (w < 1) w = 1;

    /* Thread 0 allocates the auxiliary buffer once; everyone waits */
    if (args->id == 0 && fully_par_aux == NULL)
        fully_par_aux = (int *)malloc(n * sizeof(int));
    pthread_barrier_wait(&barrier);
    if (fully_par_aux == NULL) return NULL; /* allocation failed: give up */

    int *dst = fully_par_aux;
    const int *src = args->array;

    /* ---- Phase 1: each thread sequentially sorts its own slice ---- */
    long int left = (long int)args->id * w;
    long int right = left + w - 1;
    if (right > n - 1) right = n - 1;
    if (left <= right)
        sequential_merge_sort(args->array, (int)left, (int)right);

    pthread_barrier_wait(&barrier);

    /* ---- Phase 2: iterative bottom-up merging, work at every level
     * is shared by all NB_THREADS threads ---- */
    while (w < n) {
        /* Output segment handled by this thread at this level */
        int lo_seg = (int)((long int)args->id * n / p);
        int hi_seg = (int)((long int)(args->id + 1) * n / p) - 1;
        if (hi_seg > n - 1) hi_seg = n - 1;

        for (long int start = 0; start < (long int)n; start += 2 * (long int)w) {
            int a0 = (int)start;
            int b0 = (int)(start + w);
            if (b0 > n - 1) break; /* lone final run, nothing to merge */
            int a1 = b0 - 1;
            int b1 = (int)(start + 2 * w) - 1;
            if (b1 > n - 1) b1 = n - 1;

            /* Part of this merge's output that falls in my segment */
            int out_lo = (a0 > lo_seg) ? a0 : lo_seg;
            int out_hi = (b1 < hi_seg) ? b1 : hi_seg;
            if (out_lo > out_hi)
                continue; /* this merge is handled entirely by others */

            merge_slice(src, dst, a0, a1, b0, b1, out_lo, out_hi);
        }

        pthread_barrier_wait(&barrier);

        /* Swap buffers: next level reads what was just written */
        const int *tmp = src;
        src = dst;
        dst = (int *)tmp;

        w *= 2;
    }

    /* If the final result ended up in the auxiliary buffer, copy it back */
    if (src != args->array) {
        int lo_seg = (int)((long int)args->id * n / p);
        int hi_seg = (int)((long int)(args->id + 1) * n / p) - 1;
        if (hi_seg > n - 1) hi_seg = n - 1;
        for (int k = lo_seg; k <= hi_seg; k++)
            args->array[k] = src[k];
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
