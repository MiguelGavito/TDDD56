Task 2: Multithreading with Pthreads
This task assumes that you have worked through the multithreading part of the introductory
slide set mentioned above.
Extend the sequential C program from Task 1 to a multithreaded parallel program (using
pthreads) that does the following (black text is from Task 1, blue text is new features added):
1. Read in an integer number into a variable n.
2. Dynamically allocate an array A of n floatingpoint elements.
3. Create multiple parallel pthreads, p in total including the initial thread, where p is a
program parameter. Use only p ≤ number of performance-cores of the CPU (for the
Olympen computers: 8).
In parallel,
calculate every array element A[i] = i*i+1 for all i from 0 to n-1.
(Every thread should take care of about an equal amount of elements so that the overall
work is shared across all threads.)
Then synchronize and shut down the created pthreads again (using pthread join).
4. Measure the time taken for entire step 3.
5. Output the result (A, on one thread only) and the time.
Questions:
• Considering step 5 (output of all elements of A on one thread).
Just as a thought experiment (no implementation necessary):
What would/could happen if all threads output their share of elements in parallel,
without further synchronization?
the output will be out of order
• Measure the execution time of step 3 for different values of p with fixed n.
• Compare the step-3 time of the multithreaded program (Task 2) with p = 1 threads
(i.e., no further threads created) with the corresponding time of step 3 of the sequential
program (Task 1).
• In general, parallel speedup over the sequential version by using more than one core
could only be observed for very large n (better suppress outputting all elements then).
Why is that?
