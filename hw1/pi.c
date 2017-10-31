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

typedef struct data_t{
	long numbercircle;
	long number_in_circle;
}data_t;

void *thread_Fcn(data_t *parm){
	long toss;
	int x,y;
	int distance_squared;
	

	srand(time(NULL));
	for(toss=0; toss < parm->numbercircle; toss++){
    	x = rand()%3-1;
		y = rand()%3-1;
		distance_squared = x * x + y * y ;
		if ( distance_squared <= 1)
			parm->number_in_circle ++;
    }
}

int main(int argc, char *argv[]){
    
    int toss=0;
    int rc1=0;
    double sum;
    pthread_t thread_1;
    data_t new ;
    printf("argc=%d\n",argc);
    printf("%s\n",argv[0]);
    printf("%s\n",argv[1]);
    printf("%s\n",argv[2]);


    new.numbercircle = atoi(argv[2]);

	
	rc1=pthread_create(&thread_1,NULL,thread_Fcn, &new);
    if(rc1){
        printf("ERROR thread create!");
    }
    
    rc1=pthread_join(thread_1,NULL);
    sum = 4* new.number_in_circle /(( double ) new.numbercircle ) ;
    printf("sum=%.8f\n",sum);
    printf("exit\n");

    return 0;

    
}
