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

typedef struct t_t{
time_t start;
time_t end;
}t_t;

int main(int argc, char *argv[]){
	if(argc !=3 ){ 
    	printf("parameter error:\"<./pi> <cpu_number> <run_times>\"\n");
    	return(0);
    }
    
    pthread_t   thread[atoi(argv[1])];
    data_t      new[atoi(argv[1])];
    long long   total = 0;
    long double pi = 0.0;
    t_t t_a[atoi(argv[1])]; 
    time_t t_start,t_end;
    int i;
    
    //printf("---------------------------------------------------------\nCreate thread\n");
    if(atoi(argv[1])>1){
        for(i=0; i<atoi(argv[1]); i++){
            t_a[i].start = clock();
            //t_start = clock();
            new[i].throw_num = atoll(argv[2]) / atoi(argv[1]);
            new[i].number_in_circle = 0;
            if(pthread_create(&thread[i], NULL, thread_Fcn, (void *) &new[i])) //success will return 0
                printf("ERROR:thread create!\n");
        }

        for(i=0; i<atoi(argv[1]); i++){
            if(pthread_join(thread[i],NULL))//success will return 0
            printf("ERROR:join error\n");
            t_a[i].end = clock();
            total += new[i].number_in_circle;
        }
    }
    else {
        t_a[0].start = clock();
        thread_Fcn((void *) &new[0] );
        t_a[0].end = clock();
    }

    
    for(i=0; i<atoi(argv[1]); i++){
        printf("use %f sec\n",(double)(t_a[i].end - t_a[i].start)/CLOCKS_PER_SEC);
    }
    pi = 4* total /(( long double ) atoll(argv[2]) ) ;
    printf("pi=%Lf\n",pi);
    //printf("exit\n");
    //printf("---------------------------------------------------------\n");
    return 0;

    
}
