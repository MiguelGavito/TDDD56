FILES=Makefile merge_sort.c main.c merge_sort.h
ARCHIVE=Lab2.zip

# Suggested list of features to tune to measure performance
NB_THREADS=0
ALGORITHM=0

MEASURE_FLAG=$(if $(MEASURE),-DMEASURE,)
## -O3 -msse2 -mfpmath=sse -ftree-vectorize -funroll-loops
CFLAGS= -g -O0 -Wall -DNB_THREADS=$(NB_THREADS) -DALGORITHM=$(ALGORITHM) $(MEASURE_FLAG)
## Enable this CFLAG if you want to go faster
#CFLAGS= -O3 -msse2 -mfpmath=sse -ftree-vectorize -funroll-loops  -Wall -DMAXITER=$(MAXITER) -DWIDTH=$(WIDTH) -DHEIGHT=$(HEIGHT) -DLOWER_R=$(LOWER_R) -DUPPER_R=$(UPPER_R) -DLOWER_I=$(LOWER_I) -DUPPER_I=$(UPPER_I) -DNB_THREADS=$(NB_THREADS) -DLOADBALANCE=$(LOADBALANCE) $(MEASURE_FLAG) $(DEBUG_FLAG) -DMANDELBROT_COLOR=$(MANDELBROT_COLOR) -DGLUT=$(GLUT)
LDFLAGS=-lrt -lpthread -lm

all: mergesort-$(NB_THREADS)-$(ALGORITHM)

clean:
	$(RM) mergesort-*
	$(RM) mergesort
	$(RM) *.o
mergesort-$(NB_THREADS)-$(ALGORITHM): main.c mergesort-$(NB_THREADS)-$(ALGORITHM).o
	gcc $(CFLAGS) -o mergesort-$(NB_THREADS)-$(ALGORITHM) mergesort-$(NB_THREADS)-$(ALGORITHM).o main.c $(LDFLAGS)

mergesort-$(NB_THREADS)-$(ALGORITHM).o: merge_sort.c
	gcc $(CFLAGS) -c -o mergesort-$(NB_THREADS)-$(ALGORITHM).o merge_sort.c

dist:
	zip $(ARCHIVE) $(FILES)
