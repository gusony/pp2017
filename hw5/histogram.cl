__kernel void image(__global unsigned int *T_mem_obj, __global unsigned int *R_mem_obj, __global unsigned int *G_mem_obj, __global unsigned int *B_mem_obj){
	const unsigned int idx = get_global_id(0);
	if(idx < 10)
		printf("%d:%d\n",idx,*T_mem_obj+idx);

	switch (idx mod 3){
		case 0:
			R_mem_obj[*T_mem_obj+idx]++;
			break;

		case 1:
			G_mem_obj[*T_mem_obj+idx]++;
			break;

		case 2:
			B_mem_obj[*T_mem_obj+idx]++;
			break;
	}
}