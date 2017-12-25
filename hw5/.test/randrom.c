#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]){
    long pixel=4096*3112;

    FILE *output = fopen("input","w");
    fprintf(output, "%ld\n",pixel*3);

    for(long i=0; i<pixel; i++)
    	fprintf(output,"%d %d %d\n",rand()%256,rand()%256,rand()%256);



    fclose(output);
    return(0);

}
