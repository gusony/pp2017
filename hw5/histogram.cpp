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

std::fstream inFile("input", std::ios_base::in);
std::ofstream outFile("0646001.out", std::ios_base::out);
cl_kernel kernel;
unsigned int input_size;
cl_command_queue command_queue;

#define LIST_SIZE 256

using namespace std;
void run_one_color(cl_mem *in_mem_obj, cl_mem *out_mem_obj){
    // Set the arguments of the kernel
    unsigned int status =0;
    unsigned int * histogram_results = new unsigned int[input_size/3];

    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &in_mem_obj);
    if(status != CL_SUCCESS) printf("clSetKernelArg T error %d\n",status);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &out_mem_obj);
    if(status != CL_SUCCESS) printf("clSetKernelArg R error %d\n",status);

    // Execute the OpenCL kernel on the list
    size_t global_item_size = input_size/3; // Process the entire lists
    size_t local_item_size = 64; // Divide work items into groups of 64

    status = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    if(status != CL_SUCCESS) printf("clEnqueueNDRangeKernel error %d\n",status);
    status = clEnqueueReadBuffer(command_queue, *out_mem_obj, CL_TRUE, 0,  sizeof(unsigned int)*256, histogram_results, 0, NULL, NULL);
    if(status != CL_SUCCESS) printf("clEnqueueReadBuffer error %d\n",status);

    for(unsigned int i = 0; i < 256 ; ++i) {
        if (i % 256 == 0 && i != 0)
            outFile << std::endl;
        outFile << histogram_results[i]<< ' ';
    }
    outFile << "\n";

}
int main(int argc, char const *argv[])
{

    unsigned int i=0, a;
    //unsigned int input_size;

    cl_platform_id platform_id = NULL;
    cl_device_id   device_id = NULL;
    cl_uint        ret_num_devices , ret_num_platforms;
    cl_int         status;

    FILE *fp;//open .cl file
    char *source_str;//store the .cl code string
    size_t source_size;//.cl code string len


    ////////////// read input ////////////////////////
    inFile >> input_size;
    unsigned int * histogram_results = new unsigned int[input_size/3];
    unsigned int *image_R = new unsigned int[input_size/3];//read from input file
    unsigned int *image_G = new unsigned int[input_size/3];
    unsigned int *image_B = new unsigned int[input_size/3];

    i=0;
    while( inFile >> a ) {
        image_R[i] = a;

        inFile >> a;
        image_G[i] = a;

        inFile >> a;
        image_B[i++] = a;
    }
    ////////////// read .cl file ///////////////////////////////
    fp = fopen("histogram.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    //////////////// init opencl ////////////////////////////////
    status = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    if(status != CL_SUCCESS) printf("Error: Getting Platforms\n");

    status = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
    if(status != CL_SUCCESS) printf("Error: Getting Platforms\n");

    //create context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &status);
    if(status != CL_SUCCESS) printf("clCreateContext  error %d\n",status);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &status);
    if(status != CL_SUCCESS) printf("clCreateCommandQueue error %d\n",status);

    // Create memory buffers on the device for each vector
    cl_mem init_R_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned int)*input_size/3, NULL, &status);
    if(status != CL_SUCCESS) printf("clCreateBuffer image_R error %d\n",status);
    cl_mem init_G_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned int)*input_size/3, NULL, &status);
    if(status != CL_SUCCESS) printf("clCreateBuffer image_G error %d\n",status);
    cl_mem init_B_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned int)*input_size/3, NULL, &status);
    if(status != CL_SUCCESS) printf("clCreateBuffer image_B error %d\n",status);

    cl_mem R_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int)*256, NULL, &status);
    if(status != CL_SUCCESS) printf("clCreateBuffer R error %d\n",status);
    cl_mem G_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int)*256, NULL, &status);
    if(status != CL_SUCCESS) printf("clCreateBuffer G error %d\n",status);
    cl_mem B_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int)*256, NULL, &status);
    if(status != CL_SUCCESS) printf("clCreateBuffer B error %d\n",status);

    //copy image[] to device memory "T_mem_obj"
    status = clEnqueueWriteBuffer(command_queue, init_R_mem_obj, CL_TRUE, 0, sizeof(unsigned int)*input_size/3, image_R, 0, NULL, NULL);
    if(status != CL_SUCCESS) printf("clEnqueueWriteBuffer T  error %d\n",status);
    status = clEnqueueWriteBuffer(command_queue, init_G_mem_obj, CL_TRUE, 0, sizeof(unsigned int)*input_size/3, image_G, 0, NULL, NULL);
    if(status != CL_SUCCESS) printf("clEnqueueWriteBuffer T  error %d\n",status);
    status = clEnqueueWriteBuffer(command_queue, init_B_mem_obj, CL_TRUE, 0, sizeof(unsigned int)*input_size/3, image_B, 0, NULL, NULL);
    if(status != CL_SUCCESS) printf("clEnqueueWriteBuffer T  error %d\n",status);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &status);
    if(status != CL_SUCCESS) printf("clCreateProgramWithSource  error %d\n",status);

    // Build the program
    status = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if(status != CL_SUCCESS) printf("clBuildProgram  error %d\n",status);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "adder", &status);
    if(status != CL_SUCCESS) printf("clCreateKernel  error %d\n",status);

    //run_one_color(&init_R_mem_obj, &R_mem_obj);

    // Set the arguments of the kernel
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&init_R_mem_obj);
    if(status != CL_SUCCESS) printf("clSetKernelArg T error %d\n",status);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&R_mem_obj);
    if(status != CL_SUCCESS) printf("clSetKernelArg R error %d\n",status);
    // Execute the OpenCL kernel on the list
    size_t global_item_size = input_size/3; // Process the entire lists
    //size_t local_item_size = 64; // Divide work items into groups of 64
    //status = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    status = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, 0, 0, NULL, NULL);
    if(status != CL_SUCCESS) printf("clEnqueueNDRangeKernel error %d\n",status);
    status = clEnqueueReadBuffer(command_queue, R_mem_obj, CL_TRUE, 0,  sizeof(unsigned int)*256, histogram_results, 0, NULL, NULL);
    if(status != CL_SUCCESS) printf("clEnqueueReadBuffer error %d\n",status);
    for(unsigned int i = 0; i < 256 ; ++i) {
        if (i % 256 == 0 && i != 0)
            outFile << std::endl;
        outFile << histogram_results[i]<< ' ';
    }
    outFile << "\n";

    // Set the arguments of the kernel
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&init_G_mem_obj);
    if(status != CL_SUCCESS) printf("clSetKernelArg T error %d\n",status);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&G_mem_obj);
    if(status != CL_SUCCESS) printf("clSetKernelArg R error %d\n",status);
    // Execute the OpenCL kernel on the list
    global_item_size = input_size/3; // Process the entire lists
    //size_t local_item_size = 64; // Divide work items into groups of 64
    //status = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    status = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, 0, 0, NULL, NULL);
    if(status != CL_SUCCESS) printf("clEnqueueNDRangeKernel error %d\n",status);
    status = clEnqueueReadBuffer(command_queue, G_mem_obj, CL_TRUE, 0,  sizeof(unsigned int)*256, histogram_results, 0, NULL, NULL);
    if(status != CL_SUCCESS) printf("clEnqueueReadBuffer error %d\n",status);
    for(unsigned int i = 0; i < 256 ; ++i) {
        if (i % 256 == 0 && i != 0)
            outFile << std::endl;
        outFile << histogram_results[i]<< ' ';
    }
    outFile << "\n";

    // Set the arguments of the kernel
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&init_B_mem_obj);
    if(status != CL_SUCCESS) printf("clSetKernelArg T error %d\n",status);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&B_mem_obj);
    if(status != CL_SUCCESS) printf("clSetKernelArg R error %d\n",status);
    // Execute the OpenCL kernel on the list
    global_item_size = input_size/3; // Process the entire lists
    //size_t local_item_size = 64; // Divide work items into groups of 64
    //status = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    status = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, 0, 0, NULL, NULL);
    if(status != CL_SUCCESS) printf("clEnqueueNDRangeKernel error %d\n",status);
    status = clEnqueueReadBuffer(command_queue, B_mem_obj, CL_TRUE, 0,  sizeof(unsigned int)*256, histogram_results, 0, NULL, NULL);
    if(status != CL_SUCCESS) printf("clEnqueueReadBuffer error %d\n",status);
    for(unsigned int i = 0; i < 256 ; ++i) {
        if (i % 256 == 0 && i != 0)
            outFile << std::endl;
        outFile << histogram_results[i]<< ' ';
    }
    outFile << "\n";


    inFile.close();
    outFile.close();

    return 0;
}

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
