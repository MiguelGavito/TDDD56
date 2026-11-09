#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

float *A;
float sum = 0.0;
pthread_mutex_t mutex;

struct My_IThread_Arg {
    int size, offset;
};

void *calculate_array(void *arg) {
    struct My_IThread_Arg *ptr;
    ptr = (struct My_IThread_Arg *) arg;
    float partial_sum = 0;
    for (int i = ptr->offset; i < ptr->offset + ptr->size; i++) {
        float current = i * i + 1;
        partial_sum += current;
        A[i] = current;
    }
    pthread_mutex_lock(&mutex);
    sum += partial_sum;
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char *argv[]) {
    int P;
    if (argc != 2) {
        P = 4;
    } else {
        P = atoi(argv[1]);
    }
    printf("Number of threads : %i", P);
    clockid_t clk = CLOCK_REALTIME;
    struct timespec *time1, *time2;
    time1 = (struct timespec *)malloc(sizeof(struct timespec));
    time2 = (struct timespec *)malloc(sizeof(struct timespec));
    pthread_t thr[P];

    int n;
    printf("Enter an integer n : ");
    scanf("%i", &n);
    A = malloc(n * sizeof(float));

    clock_gettime(clk, time1);
    int i;
    
    for (i = 0; i < P; i++) {
        struct My_IThread_Arg *arg = malloc(sizeof(struct My_IThread_Arg));
        arg->size = n/P;
        arg->offset = i * n/P;
        pthread_create(&(thr[i]), NULL, calculate_array, arg);
    }

    for (i = 0; i < P; i++) {
        pthread_join(thr[i], NULL);
    }
    clock_gettime(clk, time2);
    printf("\ntime [s]: %f \n", 
    (float)(time2->tv_sec - time1->tv_sec)
    + (float)(time2->tv_nsec - time1->tv_nsec) / 1000000000.0 );
    printf("Global sum: %.1f\n", sum);
    free(A);
    free(time1);
    free(time2);
    return 0;
}