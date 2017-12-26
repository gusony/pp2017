#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int64_base_atomics : enable

__kernel void adder(__global const unsigned int *image, __global unsigned int *color){
	int idx = get_global_id(0);
	atom_inc(&color[image[idx]]);
	/*
	barrier(CLK_GLOBAL_MEM_FENCE);
	color[image[idx]]++;
	barrier(CLK_GLOBAL_MEM_FENCE);
	*/
}