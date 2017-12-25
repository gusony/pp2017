#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fstream>
#include <iostream>

#include <CL/cl.h>
#include <vector>

int main(int argc, char* argv[]){
cl_int error;

    //Setup Platform
    cl_uint testPlatformCount;
    error = clGetPlatformIDs(0, NULL, &testPlatformCount);
    printf("%d\n",error);

    return(0);
}