// Simple CUDA example by Ingemar Ragnemalm 2009. Simplest possible?
// Assigns every element in an array with its index.
// Update 2022: Changed to cudaDeviceSynchronize.

// nvcc simple.cu -L /usr/local/cuda/lib -lcudart -o simple

#include <stdio.h>

const int N = 16; 
const int blocksize = 16; 

__global__ 
void simple(float *c, float * f) 
{
	c[threadIdx.x] = threadIdx.x;
	// *f = *f + 1;
	atomicAdd(f,1);
}

int main()
{
	float *input;
	float f = 0;
	//float *pzero;
	//*pzero = 0;
	
	cudaMalloc( (void**)&input, sizeof(float));
	//cudaMemcpy( input, pzero, sizeof(float), cudaMemcpyHostToDevice);

	float *c = new float[N];	
	float *cd;
	const int size = N*sizeof(float);
	
	cudaMalloc( (void**)&cd, size );
	dim3 dimBlock( blocksize, 1 );
	dim3 dimGrid( 1, 1 );
	simple<<<dimGrid, dimBlock>>>(cd, input);
	
	cudaDeviceSynchronize();
	cudaMemcpy( c, cd, size, cudaMemcpyDeviceToHost ); 
	cudaMemcpy(&f, input, sizeof(float), cudaMemcpyDeviceToHost);
	//printf("%f\n", f);
	cudaFree( cd );
	cudaFree(input);
	
	for (int i = 0; i < N; i++)
		printf("%f ", c[i]);
	printf("\n");
	printf("%f\n", f);

	delete[] c;
	printf("done\n");
	return EXIT_SUCCESS;
}
