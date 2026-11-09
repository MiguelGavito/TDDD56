# Lab 1 : Load balancing

### Before lab session

* **Why computation load can be unbalanced?**\
Each pixel is computed independently, but the computation time of each pixel can vary a lot.\
It depends if the computation converges, or at least doesn't diverge enough before MAXITER (black pixel, considered as belonging to the MAndelbrot set), or if the computation diverges, and how fast it diverges (colored pixel, the faster it diverges and reaches MAXDIV, the smaller the computation load is).\
If the pixels are evenly distributed regardless of their load between the threads, some threads may have more longer computations time pixels to process (for instance, a lot of black pixels) than others, leading to uneven computation time between threads. The total computation time is then the computation time of the longer thread, while other threads are idle.

* **Wich load balancing method to use?**\
To be discussed.