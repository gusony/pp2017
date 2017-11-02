/*
(1)input 
1:cpu number
2:執行次數

(2)scalability
可以在指定的CPU核心跑

(3)output pi estimate

(4)gcc -pthread -std=gnu99 -O2 -s pic -o pi
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_THREAD_NUM 32

typedef struct data_t{
	long long throw_num;
	long long number_in_circle;
}data_t;


void *thread_Fcn(void *parm){
	double i;
	double x,y;
    long long sum=0;
    unsigned int seed = time(NULL);
    data_t *para = (data_t *)parm;
    
	srand(time(NULL));
	for(i=0; i<para->throw_num; i++){
    	x = ((rand_r(&seed)%2000000)-1000000)/(double)1000000;
		y = ((rand_r(&seed)%2000000)-1000000)/(double)1000000;
		if ( (double)(x * x + y * y) <= 1.0)
			sum++;
    }
    para->number_in_circle = sum;
}

void exe(int thread_num, long double run_times){
    pthread_t   thread[MAX_THREAD_NUM];
    data_t      new[MAX_THREAD_NUM];
    long long   total = 0;
    long double pi = 0.0;
    int i;
    time_t t_start, t_end;
    
    t_start = clock();
    for(i=0; i<thread_num; i++){
        new[i].throw_num = run_times / thread_num;
        new[i].number_in_circle = 0;
        if(pthread_create(&thread[i], NULL, thread_Fcn, (void *) &new[i])) //success will return 0
            printf("ERROR:thread create!\n");
    }
    for(i=0; i<thread_num; i++){
        if(pthread_join(thread[i],NULL))//success will return 0
            printf("ERROR:join error\n");
        total += new[i].number_in_circle;
    }
    t_end=clock();
    pi = 4* total /( long double )run_times ;
    printf("use %f sec\n",(t_end - t_start)/(double)CLOCKS_PER_SEC);
    printf("pi=%Lf\n\n",pi);
}

int main(int argc, char *argv[]){
	if(argc !=3 ){ 
    	printf("parameter error:\"<./pi> <cpu_number> <run_times>\"\n");
    	return(0);
    }
    exe(1,atoll(argv[2]));
    exe(2,atoll(argv[2]));
    exe(4,atoll(argv[2]));
    exe(8,atoll(argv[2]));
    exe(16,atoll(argv[2]));
    exe(32,atoll(argv[2]));
    return 0;

    
}
