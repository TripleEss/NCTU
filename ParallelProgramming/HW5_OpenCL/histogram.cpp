#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fstream>
#include <iostream>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

unsigned int * histogram(unsigned int *image_data, unsigned int _size) {

	unsigned int *img = image_data;
	unsigned int *ref_histogram_results;
	unsigned int *ptr;

	ref_histogram_results = (unsigned int *)malloc(256 * 3 * sizeof(unsigned int));
	ptr = ref_histogram_results;
	memset (ref_histogram_results, 0x0, 256 * 3 * sizeof(unsigned int));

	// histogram of R
	for (unsigned int i = 0; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	// histogram of G
	ptr += 256;
	for (unsigned int i = 1; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	// histogram of B
	ptr += 256;
	for (unsigned int i = 2; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	return ref_histogram_results;
}

const char * cl_srccode = \
"__kernel void cl_histogram(__global unsigned int *img, __global unsigned int *hist)" \
"{" \
"	const int idx = get_global_id(0);" \
"	unsigned int color = img[idx];" \
"	unsigned int rgb = idx%3;" \
"	atomic_inc(&hist[256*rgb+color]);" \
"}";


int main(int argc, char const *argv[])
{

	unsigned int *histogram_results;
	unsigned int i=0, a, input_size;
	std::fstream inFile("input", std::ios_base::in);
	std::ofstream outFile("0456024.out", std::ios_base::out);

	inFile >> input_size;
	unsigned int *image = new unsigned int[input_size];
	while( inFile >> a ) {
		image[i++] = a;
	}

	histogram_results = new unsigned int[256*3];
	memset(histogram_results, 0, 256*3*sizeof(unsigned int));

	cl_platform_id cl_platform;
	cl_uint cl_plfnum;
	cl_device_id cl_device;
	cl_uint cl_dvnum;

	cl_context cl_contx;
	cl_command_queue cl_cmdq;
	cl_program cl_prog;
	cl_kernel cl_kernelfn;

	cl_int cl_err;

	cl_err = clGetPlatformIDs(1, &cl_platform, &cl_plfnum);
	//std::cout << "clGetPlatformIDs " << cl_err << std::endl;
	cl_err = clGetDeviceIDs(cl_platform, CL_DEVICE_TYPE_GPU, 1, &cl_device, &cl_dvnum);
	//std::cout << "clGetDeviceIDs " << cl_err << std::endl;
	cl_contx = clCreateContext(NULL, 1, &cl_device, NULL, NULL, &cl_err);
	//std::cout << "clCreateContext " << cl_err << std::endl;
	cl_cmdq = clCreateCommandQueue(cl_contx, cl_device, 0, &cl_err);
	//std::cout << "clCreateCommandQueue " << cl_err << std::endl;
	
	cl_mem cl_inputdata = clCreateBuffer(cl_contx, CL_MEM_READ_ONLY, input_size*sizeof(unsigned int), NULL, &cl_err);
	//std::cout << "clCreateBuffer " << cl_err << std::endl;
	cl_mem cl_outputdata = clCreateBuffer(cl_contx, CL_MEM_READ_WRITE, 256*3*sizeof(unsigned int), NULL, &cl_err);
	//std::cout << "clCreateBuffer " << cl_err << std::endl;

	cl_err = clEnqueueWriteBuffer(cl_cmdq, cl_inputdata, CL_TRUE, 0, input_size*sizeof(unsigned int), (void *)image, 0, NULL, NULL);
	//std::cout << "clEnqueueWriteBuffer " << cl_err << std::endl;
	cl_err = clEnqueueWriteBuffer(cl_cmdq, cl_outputdata, CL_TRUE, 0, 256*3*sizeof(unsigned int), (void *)histogram_results, 0, NULL, NULL);
	//std::cout << "clEnqueueWriteBuffer " << cl_err << std::endl;

	cl_prog = clCreateProgramWithSource(cl_contx, 1, (const char **)&cl_srccode, 0, &cl_err);
	//std::cout << "clCreateProgramWithSource " << cl_err << std::endl;
	cl_err = clBuildProgram(cl_prog, 0, NULL, NULL, NULL, NULL);
	//std::cout << "clBuildProgram " << cl_err << std::endl;
	cl_kernelfn = clCreateKernel(cl_prog, "cl_histogram", &cl_err);
	//std::cout << "clCreateKernel " << cl_err << std::endl;

	cl_err = clSetKernelArg(cl_kernelfn, 0, sizeof(cl_mem), (void *)&cl_inputdata);
	//std::cout << "clSetKernelArg " << cl_err << std::endl;
	cl_err = clSetKernelArg(cl_kernelfn, 1, sizeof(cl_mem), (void *)&cl_outputdata);
	//std::cout << "clSetKernelArg " << cl_err << std::endl;

	//clEnqueueTask(cl_cmdq, cl_kernelfn, 0, NULL,NULL);
	size_t localws[] = {};
	size_t globalws[] = {input_size};
	cl_err = clEnqueueNDRangeKernel(cl_cmdq, cl_kernelfn, 1, 0, globalws, NULL, 0, NULL, NULL);
	//std::cout << "clEnqueueNDRangeKernel " << cl_err << std::endl;

	clEnqueueReadBuffer(cl_cmdq, cl_outputdata, CL_TRUE, 0, 256*3*sizeof(unsigned int), (void *)histogram_results, NULL, NULL, NULL);
	//std::cout << "clEnqueueReadBuffer " << cl_err << std::endl;

	cl_err = clReleaseKernel(cl_kernelfn);
	cl_err = clReleaseProgram(cl_prog);
	cl_err = clReleaseMemObject(cl_inputdata);
	cl_err = clReleaseMemObject(cl_outputdata);
	cl_err = clReleaseCommandQueue(cl_cmdq);
	cl_err = clReleaseContext(cl_contx);

	for(unsigned int i = 0; i < 256 * 3; ++i) {
		if (i % 256 == 0 && i != 0)
			outFile << std::endl;
		outFile << histogram_results[i]<< ' ';
	}

	inFile.close();
	outFile.close();

	return 0;
}