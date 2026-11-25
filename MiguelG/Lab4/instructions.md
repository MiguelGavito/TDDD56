Lab 4
Introduction to CUDA

This lab introduces you to CUDA computing with a few small exercises. We have deliberately avoided larger amounts of code for this lab, and save that for lab 5.

Examination:

You should write down answers to all questions and then show us your results.

Deadline:

To be announced. All labs have a common deadline after the last lab.

If you have your own computer with CUDA installed, the better.

All the files for the lab are here:

lab4-2026.zip

In case of emergencies, last year’s version is here:

lab4-2025.zip

Part 1. Trying out CUDA

To get started, we will use the "simple.cu" example that I introduced in the lecture, arguably the simplest CUDA example. All it does is to assign every element in an array with its index.

a) Compile and run simple.cu

In the lab files, you find simple.cu, a program as simple as Hello World.

You can compile it with this command-line:

nvcc simple.cu -o simple

This is needed if you are in the lab:

module add courses/TDDD56

This works nicely in the lab, but for completion, let me show how a slighly more elaborate line can look:

nvcc simple.cu -lcudart -o simple

or

nvcc simple.cu -L /usr/local/cuda/lib64 -lcudart -o simple

Run with:

./simple

(Feel free to build a makefile. You can base it on those from earlier labs.)

If it works properly, it should output the numbers 0 to 15 as floating-point numbers.

As it stands, the amount of computation is ridiculously small. We will soon address problems where performance is meaningful to measure. But first, let's modify the code to get used to the format.

QUESTION: How many cores will simple.cu use, max, as written? How many SMs?

b) Modifying simple.cu

Allocate a single float, set to zero and upload as input. This is the only input/output you need. In the kernel, each thread should add one to this float.

To upload data to the GPU, you must use the cudaMemcpy() call with cudaMemcpyHostToDevice.

What number did you get, and what should you get? Now increase the grid size to 16x16. Did you get a different number than you should?

These numbers are clearly not correct. Now make it work correctly. (This is not a performance problem but a correctness problem.)

QUESTION: What was the problem and how did you fix it?



Part 2. Performance and block size

Now let us work on a larger dataset and make more computations. We will make fairly trivial item-by-item computations but experiment with the number of blocks and threads.

a) Array computation from C to CUDA

In the lab material you find a simple C program that takes two arrays (matrices) and adds them component-wise.

matrix_cpu.c

It can be compiled with

gcc matrix_cpu.c -o matrix_cpu -std=c99

and run with

./matrix_cpu

Write a CUDA program that performs the same thing, in parallel! Start with a grid size of (1, 1) and a block size of (N, N). Then try a bigger grid, with more blocks.

We must calculate an index from the thread and block numbers. That can look like this (in 1D):

int idx = blockIdx.x * blockDim.x + threadIdx.x;

QUESTION: How do you calculate the index in the array, using 2-dimensional blocks?

b) Larger data set and timing with CUDA Events

In order to measure execution time for your kernels you should use CUDA Events.

A CUDA event variable "myEvent" is declared as

  cudaEvent_t myEvent;

It must be initialized with

  cudaEventCreate(&myEvent);

You insert it in the CUDA stream with

  cudaEventRecord(myEvent, 0);

The 0 is the stream number, where 0 is the default stream.

To make sure an event have finished (received its value), call

  cudaEventSynchronize(myEvent);

Important! You must use cudaEventSynchronize before taking time measurements, or you don't know if the computation has ended!

Finally, you get the time between two events with

  cudaEventElapsedTime(&theTime, myEvent, laterEvent);

where theTime is a float.

Please note: The function cudaEventElapsedTime returns the elapsed time between two events in milliseconds.

For timing CPU code, you can use the files milli.c and milli.h. It is not unreasonable to measure GPU time too with that but you should expect it to be less precise.

You can compile milli.c on the nvcc command-line together with the main program.

Note that in CUDA, large arrays are best allocated in C++ style: float *c = new float[N*N];

Vary N. You should be able to run at least 1024x1024 items.

Vary the block size (and consequently the grid size).

Non-mandatory: Do you find the array initialization time consuming? Write a kernel for that, too!

QUESTION: What happens if you use too many threads per block?

Note: Check this out:

    cudaError_t err = cudaGetLastError();

    if (err != cudaSuccess)

        printf("Error: %s\n", cudaGetErrorString(err));

Is this related to the last question? Why?

Note: You can get misleading output if you don't clear your buffers. Old data from earlier computations will remain in the memory.

Also note that timings can be made with or without data transfers. In a serious computations, the data transfers certainly are important, but for our small examples, it is more relevant to take the time without data transfers, or both with and without.

Note: When you increase the data size, there are (at least) two problems that you will run into:

The maximum number of threads per block is limited, typically 1024 om current GPUs. If you use more, your computation will be incomplete or unpredictable. The limitations of the platform you work on can (and should in a serious application) be inspected by a "device query", using cudaGetDeviceProperties(). This includes number of threads (again, 512), number of blocks in a grid (high!) and amount of shared memory (typically 16k).

The C-style static allocation of arrays will not work for 1024x1024. You can switch to C++-style (new) or malloc.

QUESTION: At what data size is the GPU faster than the CPU?

QUESTION: What block size seems like a good choice? Compared to what?

QUESTION: Write down your data size, block size and timing data for the best GPU performance you can get.

c) Coalescing

In this part, we will make a simple experiment to inspect the impact of memory coalescing, or the lack of it.

In part 2, you calculated a formula for accessing the array items based on thread and block indices. Probably, a change in X resulted in one step in the array. That will make data aligned and you get good coalescing. Swap the X and Y calculations so a change in Y results in one step in the array. Measure the computation time.

Try for a few different cases, different size, different steps.

Note: The results have varied a lot in this exercise. I get a very significant differences, but everybody don’t get the same result. Data size makes a difference here.

QUESTION: How much performance did you lose by making data accesses non-coalesced?



Part 3. Mandelbrot revisited

In Lab 1, you worked with fractals, the Mandelbrot fractal to be precise. Here we will work with that again, although not focused on load balancing but the task of adapting the algorithm for CUDA (just like we did for the previous exercise).



Again you start with a CPU implementation. It is somewhat similar to the one from Lab 1, similar controls and all, but a bit simplified. It also uses a complex number class in order to get some operator overloading. Remember, CUDA is really based on C++, and classes are legal even in the kernel code!

Apply what you learned earlier to parallelize the Mandelbrot, turn for-loops to parallelism!

We start with the C++ version, to work from, called interactiveMandelbrot-cpu.cpp. For this part of the lab, we have a makefile. It is very straightforward. It will complain that it can not find interactiveMandelbrot-gpu.cu, because you have not created it yet.

milli.c is used as timer for the CPU code, and of course the GPU code should have it too. Experiment with the iteration depth. Also try with double precision.

Here you may need one more CUDA event call: cudaEventDestroy(), to clean up after a CUDA event and avoid memory leaks.

QUESTION: What were the main changes in order to make the Mandelbrot run in CUDA?

QUESTION: How many blocks and threads did you use?

QUESTION: When you use the Complex class, what modifier did you have to use on the methods?

QUESTION: What performance did you get? How does that compare to the CPU solution?

QUESTION: What performance did you get with float vs double precision?

QUESTION: In Lab 1, load balancing was an important issue. Is that an issue here? Why/why not?



That is all for lab 4. Write down answers to all questions and then show your results to us.