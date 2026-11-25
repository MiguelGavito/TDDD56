// Matrix addition , GPU version
// nvcc matrix_gpu.cu -L /usr/local/cuda/lib -lcudart -o matrix_gpu

#include <cstdlib>
#include <stdio.h>

const int N = 16;
const int blocksize = 16;

__global__
void add_matrix(float *a, float *b, float *c, int N)
{
	int index;
	
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			index = i + j*N;
			c[index] = a[index] + b[index];
		}
}

int main()
{
    float a[N*N];
    float b[N*N];
    float c[N*N];

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			a[i+j*N] = 10 + i;
			b[i+j*N] = (float)j / N;
		}
	
    add_matrix(float *a, float *b, float *c, int N);

    for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf("%0.2f ", c[i+j*N]);
		}
		printf("\n");
	}
    
    return EXIT_SUCCESS;
}