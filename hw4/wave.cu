/**********************************************************************
 * DESCRIPTION:
 *   Serial Concurrent Wave Equation - C Version
 *   This program implements the concurrent wave equation
 *********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAXPOINTS 1000000
#define MAXSTEPS 1000000
#define MINPOINTS 20
#define PI 3.14159265
#define sqtau (1.0 * 0.3 / 1.0)*(1.0 * 0.3 / 1.0)


void check_param(void);
void init_line(void);
void update (void);
void printfinal (void);

int nsteps,                   /* number of time steps */
    tpoints,                  /* total points along string */
    rcode;                    /* generic return code */
float  values[MAXPOINTS+2],   /* values at time t */
       oldval[MAXPOINTS+2],   /* values at time (t-dt) */
       newval[MAXPOINTS+2];   /* values at time (t+dt) */


/**********************************************************************
 * Checks input values from parameters
 *********************************************************************/
void check_param(void)
{
   char tchar[20];

   /* check number of points, number of iterations */
   while ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS)) {
      printf("Enter number of points along vibrating string [%d-%d]: "
           ,MINPOINTS, MAXPOINTS);
      scanf("%s", tchar);
      tpoints = atoi(tchar);
      if ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS))
         printf("Invalid. Please enter value between %d and %d\n",
                 MINPOINTS, MAXPOINTS);
   }
   while ((nsteps < 1) || (nsteps > MAXSTEPS)) {
      printf("Enter number of time steps [1-%d]: ", MAXSTEPS);
      scanf("%s", tchar);
      nsteps = atoi(tchar);
      if ((nsteps < 1) || (nsteps > MAXSTEPS))
         printf("Invalid. Please enter value between 1 and %d\n", MAXSTEPS);
   }

   printf("Using points = %d, steps = %d\n", tpoints, nsteps);

}

/**********************************************************************
 *     Initialize points on line
 *********************************************************************/
void init_line(void)
{
   int j;
   float x, fac, k, tmp;
   time_t start = clock();

   /* Calculate initial values based on sine curve */
   fac = 2.0 * PI;
   k = 0.0;
   tmp = tpoints - 1;

   for (j = 1; j <= tpoints; j++) {
      x = k/tmp;
      values[j] = sin (fac * x);
      oldval[j] = values[j];
      k = k + 1.0;
   }
   printf("init_line:%ld\n",clock() - start);
}

/**********************************************************************
 *      Calculate new values using wave equation
 *********************************************************************/
void do_math(int i)
{
   //float dtime, c, dx, tau, sqtau;

   /*dtime = 0.3;
   c = 1.0;
   dx = 1.0;
   tau = (c * dtime / dx);
   sqtau = tau * tau;*/


   newval[i] = (2.0 * values[i]) - oldval[i] + (sqtau *  (-2.0)*values[i]);
}
__global__ void g_do_math(float *values, float *oldval, float *newval,int nsteps, int tpoints){
	int i = 0;
	int j = blockIdx.x*32 + threadIdx.x;

	for (i = 1; i<= nsteps; i++) {
		*(newval+1) = *(newval+tpoints) = 0.0;
		*(newval+j) = (2.0 * (*(values+j)) ) - (*(oldval+j)) + (sqtau *  (-2.0)* (*(values+j)) );

		*(oldval+j) = *(values+j);
		*(values+j) = *(newval+j);
	}
}

/**********************************************************************
 *     Update all values along line a specified number of times
 *********************************************************************/
void update()
{
   	int blocknum = ((int)(tpoints/32))+1;

    float *d_values, *d_oldval, *d_newval;
    cudaMalloc((void**)&d_values, sizeof(float) * (MAXPOINTS+2)); // values at time t
    cudaMalloc((void**)&d_oldval, sizeof(float) * (MAXPOINTS+2)); // values at time (t-dt)
    cudaMalloc((void**)&d_newval, sizeof(float) * (MAXPOINTS+2)); // values at time (t+dt)

    cudaMemcpy(d_values, &values[0], sizeof(float) * MAXPOINTS+2, cudaMemcpyHostToDevice);
	cudaMemcpy(d_oldval, &oldval[0], sizeof(float) * MAXPOINTS+2, cudaMemcpyHostToDevice);
	cudaMemcpy(d_newval, &newval[0], sizeof(float) * MAXPOINTS+2, cudaMemcpyHostToDevice);

	g_do_math<<<blocknum, 32>>>(d_values, d_oldval, d_newval, nsteps, tpoints);

	cudaMemcpy(&values[0], d_values, sizeof(float) * MAXPOINTS+2, cudaMemcpyDeviceToHost);
    cudaMemcpy(&oldval[0], d_oldval, sizeof(float) * MAXPOINTS+2, cudaMemcpyDeviceToHost);
	cudaMemcpy(&newval[0], d_newval, sizeof(float) * MAXPOINTS+2, cudaMemcpyDeviceToHost);
	cudaFree(d_values);
	cudaFree(d_oldval);
	cudaFree(d_newval);
}

/**********************************************************************
 *     Print final results
 *********************************************************************/
void printfinal()
{
   int i;

   for (i = 1; i <= tpoints; i++) {
      printf("%6.4f ", values[i]);
      if (i%10 == 0)
         printf("\n");
   }
}

/**********************************************************************
 * Main program
 *********************************************************************/
int main(int argc, char *argv[])
{
   sscanf(argv[1],"%d",&tpoints);
   sscanf(argv[2],"%d",&nsteps);
   check_param();
   printf("Initializing points on the line...\n");
   init_line();
   printf("Updating all points for all time steps...\n");
   update();
   printf("Printing final results...\n");
   //printfinal();
   printf("\nDone.\n\n");

   return 0;
}