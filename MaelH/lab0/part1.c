#include <stdio.h>
#include <time.h>
#include <stdlib.h>

float *A;

int main() {

    clockid_t clk = CLOCK_REALTIME;
    struct timespec *time1, *time2;
    time1 = (struct timespec *)malloc(sizeof(struct timespec));
    time2 = (struct timespec *)malloc(sizeof(struct timespec));

    int n;
    printf("Enter an integer n : ");
    scanf("%d", &n);
    A = (float *)malloc(n * sizeof(float));
    clock_gettime(clk, time1);
    int i;
    for (i = 0; i < n; i++) {
        A[i] = i * i + 1;
    }
    clock_gettime(clk, time2);

    printf("\ntime [s]: %f \n", 
    (float)(time2->tv_sec - time1->tv_sec)
    + (float)(time2->tv_nsec - time1->tv_nsec) / 1000000000.0 );
    printf("Adress of i : %p\n", &i);
    printf("Adress of A[0] : %p\n", &A[0]);
    printf("Adress of A[1] : %p\n", &A[1]);
    printf("Adress of A : %p\n", A);
    printf("Adress of main : %p\n", &main);
    printf("Size of A : %u\n", sizeof(A));
    printf("Size of A[0] : %u\n", sizeof(A[0]));
    printf("Size of i : %u\n", sizeof(i));
    printf("A[1] converted to an integer : %d\n", (int)A[1]);
    printf("A[1] interpreted as an integer : %d\n", *(int *)&A[1]);
    return 0;
}