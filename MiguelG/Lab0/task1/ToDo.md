Task 1: C Pointers and Memory
This lab assumes that you have worked through the C part of the slide set mentioned above.
Write a sequential C program that does the following:
1. Read in an integer number into a variable n.
2. Dynamically allocate an array A (as a local pointer variable in main) of n floatingpoint
elements.
3. Calculate every array element A[i] = i*i+1 for all i from 0 to n-1.
4. Measure the time taken for entire step 3. (The functions can be found on the lab page
and an example is in the slide set.)
5. Output the result and the time.
Additional experiments:
6. Also output the addresses of i, A[0], A[1], A, and main.
7. Use a global pointer variable for A instead and redo step 6.
8. Output the size (in bytes) of i, of A[0], and of A.
9. What happens if you read from or write to A out of bounds?
Start to read out of the array and can make segmentation fault and bus error
10. Using suitable type casting, convert A[1] into an integer value and print it.
11. Using suitable type casting, reinterpret A[1] as an integer value and print it.
Questions
• How many bytes does a pointer variable have on your system?
the pointer A use 8 bytes
• Why do we here need to use a pointer and dynamic memory allocation for A (step 2
resp. step 7), instead of simply defining a C array variable for it?
Using the pointer and dynamic memory allows us to modify the size of the array after creating it,and if we dont use the pointer the direction of the A and A[0] start in the same place of direction.
How could this be changed if n were a compiler-known constant?
if n were a compiler known consntant the dynamic memory allocation is not that needed no mater when we receive the n variable, before we needed to define first the n and then create the array and with the global pointer we could create first the array but if is a constant we can just use a normal array.
Write a variant of this program to work with a constant n (and thus a fixed-size array).
• Run the entire program with execution time measurement (step 5) multiple times.
How much variation of the time values do you observe across the runs? What could
be the reason(s)?
First i checheck taht with a constant n is more fast the time, altmost 50% more fast than the dynamic, the diference in time and directions is because each run is not in the same address so in eeach run the optimziation of the compiler vary a little and make little diference in how the memory is managed, when i tried to read and write outside sometimes was bus error, others segmentation fault and sometimes worked, but that was only because how the memory was managed in the run, some sizes maked it more possible for show the failures of not. the reason of the variation are the other fctors in the run, the cpu, how much cache is used or free, the threaths and things that are changing constantly.
the time is around 0.000004 0.000003
