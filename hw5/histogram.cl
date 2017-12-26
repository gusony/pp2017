__kernel void adder(__global const unsigned int *image, __global unsigned int *color){
	int idx = get_global_id(0);
	color[image[idx]]++;
}