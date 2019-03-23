#include "types.h"
#include "stat.h"
#include "user.h"


// #define N 11
// #define E 0.00001
// #define T 100.0
// #define P 6
// #define L 20000

float fabsm(float a){
	if(a<0)
	return -1*a;
return a;
}
float stof(char* s){
  float rez = 0, fact = 1;
  if (*s == '-'){
    s++;
    fact = -1;
  };
  for (int point_seen = 0; *s; s++){
    if (*s == '.'){
      point_seen = 1; 
      continue;
    };
    int d = *s - '0';
    if (d >= 0 && d <= 9){
      if (point_seen) fact /= 10.0f;
      rez = rez * 10.0f + (float)d;
    };
  };
  return rez * fact;
};

float readfloat(int fdfile){
    int maxsize=100;
    // char arr[maxsize];
    char * arr =(char *)malloc(sizeof(char)*maxsize);
    float ans=0;
    char c;
    for(int i=0;i<maxsize;i++){
        read(fdfile,&c,1);
        if(c=='\n'){
            arr[i]='\0';
            break;
        }
        arr[i]=c;
    }
    // printf(1,"%s is what I read\n",arr);
    ans = stof(arr);
    // printf(1,"%d is ans\n",(int) (ans*1000000));
    return ans;
}
int readint(int fdfile){
    int maxsize=100;
    char c;
    int ans=0;
    for(int i=0;i<maxsize;i++){
        read(fdfile,&c,1);
        if(c=='\n'){
            break;
        }
        // printf(1,"%c is char\n",c);
        int temp=c-'0';
        // printf(1,"%d is temp\n",temp);
        ans = 10*ans+temp;

    }
    return ans;
}
int main(int argc, char *argv[])
{
    int N=0;
	float E=0;
	float T=0;
	int P=0;
	int L=0;
    char * filename;
    filename = "assig2a.inp";
    int fdfile = open(filename,0);
    N = readint(fdfile);
    E = readfloat(fdfile);
    T = readfloat(fdfile);
    P = readint(fdfile);
    if(P==-1){
        exit();
    }
    L = readint(fdfile);
    close(fdfile);
    // printf(1,"%d is N\n",N);
    // exit();
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
	for(i =0; i <N; i++){
		for(j = 0; j<N; j++)
			printf(1,"%d ",((int)u[i][j]));
		printf(1,"\n");
	}
	exit();

}
