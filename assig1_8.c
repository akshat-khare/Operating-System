#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	if(argc< 2){
		printf(1,"Need type and input filename\n");
		exit();
	}
	char *filename;
	filename=argv[2];
	int type = atoi(argv[1]);
	printf(1,"Type is %d and filename is %s\n",type, filename);

	int tot_sum = 0;	
	float variance = 0.0;

	int size=1000;
	short arr[size];
	char c;
	int fd = open(filename, 0);
	for(int i=0; i<size; i++){
		read(fd, &c, 1);
		arr[i]=c-'0';
		read(fd, &c, 1);
	}	
  	close(fd);
  	// this is to supress warning
  	printf(1,"first elem %d\n", arr[0]);
  
  	//----FILL THE CODE HERE for unicast sum and multicast variance
	// int cidarr[8];
	int ansarr[8];
	int masterpid=getpid();
	int numthreads=4;
	for(int i=0;i<numthreads;i++){
		int cid=fork();
		if(cid==0){
			int sumlocal=0;
			int start = i*(1000/numthreads);
			int end;
			if(i==numthreads-1){
				end=1000;
			}else{
				end = (i+1)*(1000/numthreads);
			}
			for(int j=start;j<end;j++){
				sumlocal=add(sumlocal,arr[j]);
			}
			char * mess = (char *) (&sumlocal);
			// printf(1,"child %d sending sumlocal %d\n",i,sumlocal);
			int stat=-1;
			while(stat!=0){
				stat=send(cid,masterpid,mess);
			}
			exit();
		}else{
			// cidarr[i] =cid;
		}
	}
	for(int i=0;i<numthreads;i++){
		char *msg = (char *)malloc(MSGSIZE);
		int stat=-1;
		while(stat==-1){
			// printf(1,"looping while \n");
			stat = recv(msg);
		}

		int* temptotal = (int *) msg;
		// printf(1,"master iteration %d adding %d\n",i,*temptotal);
		// tot_sum += *temptotal;
		tot_sum=add(tot_sum, *temptotal);
		ansarr[i]=tot_sum;
		if(i>0){
			if(ansarr[i]==ansarr[i-1]){
				printf(1,"problem in %d iteration\n",i);
			}
		}
		// ansarr[i]=*temptotal;
	}
	// int tempsum=0;
	// for(int i=0;i<1000;i++){
	// 	tempsum += arr[i];
	// }
	// printf(1,"correct sum is %d\n", tempsum);










  	//------------------

  	if(type==0){ //unicast sum
		printf(1,"Sum of array for file %s is %d\n", filename,tot_sum);
	}
	else{ //mulicast variance
		printf(1,"Variance of array for file %s is %d\n", filename,(int)variance);
	}
	exit();
}
