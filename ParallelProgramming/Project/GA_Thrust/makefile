NVCC=nvcc
OFLAG=-D_FORCE_INLINES -O2 
GFLAG=-g -G -Xcompiler -fPIC -DDEBUG 

TDS=THRUST_DEVICE_SYSTEM
DTDS=-D$(TDS)=$(TDS)

CPPFLAG=$(DTDS)_CPP
OMPFLAG=-Xcompiler -fopenmp $(DTDS)_OMP -lgomp
CUDAFLAG=$(DTDS)_CUDA

FILE=GA_Thrust.cu

all:
	make cpp omp cuda

cpp:
	$(NVCC) $(OFLAG) $(CPPFLAG) $(FILE) -o bin/cpp.out
cpp_g:
	$(NVCC) $(GFLAG) $(CPPFLAG) $(FILE) -o bin/cpp.out

omp:
	$(NVCC) $(OFLAG) $(OMPFLAG) $(FILE) -o bin/omp.out
omp_g:
	$(NVCC) $(GFLAG) $(OMPFLAG) $(FILE) -o bin/omp.out

cuda:
	$(NVCC) $(OFLAG) $(CUDAFLAG) $(FILE) -o bin/cuda.out
cuda_g:
	$(NVCC) $(GFLAG) $(CUDAFLAG) $(FILE) -o bin/cuda.out
