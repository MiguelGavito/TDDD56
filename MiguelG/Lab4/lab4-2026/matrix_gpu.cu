// Matrix addition , GPU version
// nvcc matrix_gpu.cu -L /usr/local/cuda/lib -lcudart -o matrix_gpu

#include <stdio.h>

const int N = 16;
const int blocksize = N;

__global__
void add_matrix(float *a, float *b, float *c, int N)
{
	int xindex = blockIdx.x * blockDim.x + threadIdx.x;
	int yindex = blockIdx.y * blockDim.y + threadIdx.y;
	int index = xindex + yindex * blocksize;
	
	c[index] = a[index] + b[index];
}

int main()
{
    float a[N*N];
    float b[N*N];
    float c[N*N];

	float * ga;
	float * gb;
	float * gc;

	cudaMalloc((void**)&ga, N*N*sizeof(float));
	cudaMalloc((void**)&gb, N*N*sizeof(float));
	cudaMalloc((void**)&gc, N*N*sizeof(float));

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			a[i+j*N] = 10 + i;
			b[i+j*N] = (float)j / N;
		}
	
	cudaMemcpy(ga, a, N*N*sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(gb, b, N*N*sizeof(float), cudaMemcpyHostToDevice);

	dim3 dimBlock( blocksize, blocksize );
	dim3 dimGrid( 1, 1 );

	
    add_matrix<<<dimGrid, dimBlock>>>(ga,gb,gc,N);

	cudaMemcpy(&c, gc, N*N*sizeof(float), cudaMemcpyDeviceToHost);
	cudaFree(gc);

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