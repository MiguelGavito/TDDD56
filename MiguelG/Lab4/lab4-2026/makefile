all :  cpu gpu

cpu: interactiveMandelbrot-cpu.cpp simplefont.c milli.c
	g++ -Wall -o interactiveMandelbrot-cpu -DGL_GLEXT_PROTOTYPES interactiveMandelbrot-cpu.cpp simplefont.c  milli.c -lX11 -lm -lGL -lglut

gpu: interactiveMandelbrot-gpu.cu simplefont.c  milli.c
	nvcc -o interactiveMandelbrot-gpu -DGL_GLEXT_PROTOTYPES interactiveMandelbrot-gpu.cu simplefont.c  milli.c -lX11 -lm -lGL -lglut

clean :
	rm interactiveMandelbrot-cpu interactiveMandelbrot-gpu

