// #include "types.h"
// #include "stat.h"
// #include "user.h"

#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h> 

// #define N 20
// #define E 0.00001
// #define T 100.0
// #define P 4
// #define L 20000

float fabsm(float a){
	if(a<0)
	return -1*a;
return a;
}
int main(int argc, char *argv[])
{
	int N=0;
	float E=0;
	float T=0;
	int P=0;
	int L=0;
	FILE* file = fopen ("assig2a.inp", "r");
	fscanf (file, "%d", &N);
    fscanf (file, "%f", &E);
    fscanf (file, "%f", &T);
    fscanf (file, "%d", &P);
    fscanf (file, "%d", &L);
	fclose(file);
	float diff;
	int i,j;
	float mean;
	float u[N][N];
	float w[N][N];

	int count=0;
	mean = 0.0;
	for (i = 0; i < N; i++){
		u[i][0] = u[i][N-1] = u[0][i] = T;
		u[N-1][i] = 0.0;
		mean += u[i][0] + u[i][N-1] + u[0][i] + u[N-1][i];
	}
	mean /= (4.0 * N);
	for (i = 1; i < N-1; i++ )
		for ( j= 1; j < N-1; j++) u[i][j] = mean;
	for(;;){
		diff = 0.0;
		for(i =1 ; i < N-1; i++){
			for(j =1 ; j < N-1; j++){
				w[i][j] = ( u[i-1][j] + u[i+1][j]+
					    u[i][j-1] + u[i][j+1])/4.0;
				if( fabsm(w[i][j] - u[i][j]) > diff )
					diff = fabsm(w[i][j]- u[i][j]);	
			}
		}
	    count++;
	       
		if(diff<= E || count > L){ 
			break;
		}
	
		for (i =1; i< N-1; i++)	
			for (j =1; j< N-1; j++) u[i][j] = w[i][j];
	}
	// printf("count is %d\n",count);
	for(i =0; i <N; i++){
		for(j = 0; j<N; j++)
			printf("%d ",((int)u[i][j]));
		printf("\n");
	}
	exit(0);

}
