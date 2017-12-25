/*
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <string.h>
#include <CL/cl.h>

static const size_t vectorSize = 4096; //must be evenly disible by workSize
static const size_t workSize = 256;
#define MAX_SOURCE_SIZE (0x100000)


#define LIST_SIZE 256

/*unsigned int * histogram(unsigned int *image_data, unsigned int _size) {

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
}*/

int main(int argc, char const *argv[])
{
	cl_platform_id platform_id = NULL;
    cl_uint        ret_num_platforms;

    cl_device_id   device_id = NULL;
    cl_uint        ret_num_devices;

    cl_int         status;


	unsigned int * histogram_results;
	unsigned int i=0, a, input_size;
	std::fstream inFile("input", std::ios_base::in);
	std::ofstream outFile("0646001.out", std::ios_base::out);

	////////////// read input ////////////////////////
	inFile >> input_size;
	unsigned int *image = new unsigned int[input_size];
	while( inFile >> a ) {
		image[i++] = a;
	}
	////////////// read .cl file ////////////////////////////////
	FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("histogram.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp   );

	//////////////// init opencl ////////////////////////////////
	status = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	if(status != CL_SUCCESS)
	    printf("Error: Getting Platforms\n");

	status = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
	if(status != CL_SUCCESS)
	    printf("Error: Getting Platforms\n");


	// Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &status);
    if(status != CL_SUCCESS) printf("clCreateContext  error %d\n",status);


    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &status);
    if(status != CL_SUCCESS) printf("clCreateCommandQueue error %d\n",status);

    // Create memory buffers on the device for each vector
    cl_mem T_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned int)*input_size, NULL, &status);
    if(status != CL_SUCCESS) printf("clCreateBuffer T error %d\n",status);
    cl_mem R_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int)*256, NULL, &status);
    if(status != CL_SUCCESS) printf("clCreateBuffer R error %d\n",status);
    cl_mem G_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int)*256, NULL, &status);
    if(status != CL_SUCCESS) printf("clCreateBuffer G error %d\n",status);
    cl_mem B_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int)*256, NULL, &status);
    if(status != CL_SUCCESS) printf("clCreateBuffer B error %d\n",status);

    //copy image[] to device memory "T_mem_obj"
    status = clEnqueueWriteBuffer(command_queue, T_mem_obj, CL_TRUE, 0, sizeof(unsigned int)*input_size, image, 0, NULL, NULL);
    if(status != CL_SUCCESS) printf("clEnqueueWriteBuffer T  error %d\n",status);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &status);
    if(status != CL_SUCCESS) printf("clCreateProgramWithSource  error %d\n",status);

    // Build the program
    status = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if(status != CL_SUCCESS) printf("clBuildProgram  error %d\n",status);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "image", &status);
    if(status != CL_SUCCESS) printf("clCreateKernel  error %d\n",status);

    // Set the arguments of the kernel
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&T_mem_obj);
    if(status != CL_SUCCESS) printf("clSetKernelArg T error %d\n",status);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&R_mem_obj);
    if(status != CL_SUCCESS) printf("clSetKernelArg R error %d\n",status);
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&G_mem_obj);
    if(status != CL_SUCCESS) printf("clSetKernelArg G error %d\n",status);
    status = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&B_mem_obj);
    if(status != CL_SUCCESS) printf("clSetKernelArg B error %d\n",status);

    // Execute the OpenCL kernel on the list
    size_t global_item_size = LIST_SIZE; // Process the entire lists
    size_t local_item_size = 16; // Divide work items into groups of 64
    status = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    if(status != CL_SUCCESS)
    	printf("clEnqueueNDRangeKernel error %d\n",status);

    status = clEnqueueReadBuffer(command_queue, R_mem_obj, CL_TRUE, 0,  sizeof(unsigned int)*257, histogram_results, 0, NULL, NULL);
    if(status != CL_SUCCESS)
    	printf("clEnqueueReadBuffer error %d\n",status);

    for(unsigned int i = 0; i < 256 ; ++i) {
		if (i % 256 == 0 && i != 0)
			outFile << std::endl;
		outFile << histogram_results[i]<< ' ';
	}
	outFile << "\n";

	status = clEnqueueReadBuffer(command_queue, G_mem_obj, CL_TRUE, 0,  sizeof(unsigned int)*257, histogram_results, 0, NULL, NULL);
	for(unsigned int i = 0; i < 256 ; ++i) {
		if (i % 256 == 0 && i != 0)
			outFile << std::endl;
		outFile << histogram_results[i]<< ' ';
	}
	outFile << "\n";

    status = clEnqueueReadBuffer(command_queue, B_mem_obj, CL_TRUE, 0,  sizeof(unsigned int)*257, histogram_results, 0, NULL, NULL);
    for(unsigned int i = 0; i < 256 ; ++i) {
		if (i % 256 == 0 && i != 0)
			outFile << std::endl;
		outFile << histogram_results[i]<< ' ';
	}

	inFile.close();
	outFile.close();

	return 0;
}
