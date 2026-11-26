
#include <stdio.h>
#include <cuda_runtime.h>

#define CHECK(call) do { \
    cudaError_t err = call; \
    if (err != cudaSuccess) { \
        printf("Error en %s (línea %d): %s\n", \
               #call, __LINE__, cudaGetErrorString(err)); \
        return 1; \
    } \
} while (0)

__global__ void add(int *a, int *b, int *c) {
    *c = *a + *b;
}

int main() {
    int a = 10;
    int b = 20;
    int c = 0;

    int *d_a, *d_b, *d_c;

    // Reservar memoria en GPU
    cudaMalloc(&d_a, sizeof(int));
    cudaMalloc(&d_b, sizeof(int));
    cudaMalloc(&d_c, sizeof(int));

    // Copiar datos a GPU
    cudaMemcpy(d_a, &a, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, &b, sizeof(int), cudaMemcpyHostToDevice);

    // Ejecutar un solo hilo en un solo bloque
    add<<<1, 1>>>(d_a, d_b, d_c);

    CHECK(cudaGetLastError());
    CHECK(cudaDeviceSynchronize());

    // Esperar a que termine el kernel
    cudaDeviceSynchronize();

    // Recuperar resultado
    cudaMemcpy(&c, d_c, sizeof(int), cudaMemcpyDeviceToHost);

    printf("%d + %d = %d\n", a, b, c);

    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);

    return 0;
}

