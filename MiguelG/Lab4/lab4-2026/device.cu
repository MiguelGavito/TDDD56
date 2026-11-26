#include <stdio.h>
#include <cuda_runtime.h>

int main() {
    int runtime = 0;
    int driver = 0;
    int devices = 0;

    cudaRuntimeGetVersion(&runtime);
    cudaDriverGetVersion(&driver);

    printf("CUDA Runtime: %d\n", runtime);
    printf("CUDA Driver API: %d\n", driver);

    cudaError_t err = cudaGetDeviceCount(&devices);

    if (err != cudaSuccess) {
        printf("Device error: %s\n", cudaGetErrorString(err));
        return 1;
    }

    printf("GPU count: %d\n", devices);
    return 0;
}
