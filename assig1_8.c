#include "types.h"
#include "stat.h"
#include "user.h"
#define NUMTHREADS 4
int brother=-1;
void handlesigint(int sinum){
    // printf(1, "Awakened\n");
    char *msgmulti = (char *)malloc(MSGSIZE);
    recvmulti(msgmulti);
	// int * tempmean = (int *)msgmulti;
    // printf(1,"Message is %s or %d\n",msgmulti,*tempmean);
    // int brother=0;
    int pid=getpid();
    // int temp=0;
    // for(int i=0;i<NUMTHREADS;i++){
    //     printf(1,"brother sister----%d %d---\n", cidarr[i],cidarrsister[i]);
    // }
    // for(int i=0;i<NUMTHREADS;i++){
    //     if(cidarrsister[i]==pid){
    //         brother=cidarr[i];
    //         break;
    //     }
    // }
    // printf(1,"Sending from sister %d to brother %d\n",pid, brother);
    send(pid,brother,msgmulti);
    exit();
}
void signal(int typesig, void (*handler)(int)){
    // for(int i=0;i<NUMTHREADS;i++){
    //     printf(1,"brother sister----%d %d---\n", cidarr[i],cidarrsister[i]);
    // }
    int pid = getpid();
    int cid=fork();
    if(cid==0){
        int myrealpid = getpid();
        // for(int i=0;i<NUMTHREADS;i++){
        //     if(cidarr[i]==pid){
        //         cidarrsister[i]=myrealpid;
        //         break;
        //     }
        // }
        brother=pid;
        // for(int i=0;i<NUMTHREADS;i++){
        //     printf(1,"brother sister----%d %d---\n", cidarr[i],cidarrsister[i]);
        // }
        // printf(1,"Calling register handler for pid, cid %d %d\n",pid,myrealpid);
        registerhandler(pid,myrealpid);
        // printf(1,"After registerhandler for pid, cid %d %d\n",pid,myrealpid);
        handler(typesig);
    }else{
        // printf(1,"Signal registering for pid, cid %d, %d\n",pid,cid);
        // int temp=0;
        // for(int i=0;i<NUMTHREADS;i++){
        //     if(cidarr[i]==pid){
        //         cidarrsister[i]=cid;
        //         break;
        //     }
        // }
        brother=pid;
    }
}
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
	int signaldone[NUMTHREADS];
    int cidarr[NUMTHREADS];
    for(int i=0;i<NUMTHREADS;i++){
        signaldone[i]=0;
        // cidarr[i]=-1;
        // cidarrsister[i]=-1;
    }
    // int* cidarr = (int *)malloc(NUMTHREADS);
    for(int i=0;i<NUMTHREADS;i++){
        cidarr[i]=-2;
    }
	for(int i=0;i<NUMTHREADS;i++){
		int cid=fork();
		if(cid==0){
			int sumlocal=0;
			int start = i*(1000/NUMTHREADS);
			int end;
			if(i==NUMTHREADS-1){
				end=1000;
			}else{
				end = (i+1)*(1000/NUMTHREADS);
			}
			for(int j=start;j<end;j++){
				sumlocal=add(sumlocal,arr[j]);
			}
			char * mess = (char *) (&sumlocal);
			// printf(1,"child %d sending sumlocal %d mess %s\n",i,sumlocal, mess);
			int stat=-1;
			while(stat!=0){
				stat=send(cid,masterpid,mess);
			}


			//variance part
			if(type==1){

				cidarr[i]=getpid();
				brother=getpid();
				// for(int i=0;i<NUMTHREADS;i++){
				//     printf(1,"brother sister----%d %d---\n", cidarr[i],cidarrsister[i]);
				// }
				signal(1,handlesigint);
				// cidarr[i] = getpid();
				char *msgbrother = (char *)malloc(MSGSIZE);
				stat=-1;
				while(stat==-1){
				// signaldone[i]=1;
					stat=recv(msgbrother);
				}
				int * tempsumptr = (int *)msgbrother;
				int tempsum = *tempsumptr;
				// printf(1,"sum brother got is %d\n",tempsum);
				//processvariance
				float varlocal=0.0;
				float tempmean = (1.0*tempsum)/1000;
				for(int j=start;j<end;j++){
					varlocal += (1.0*arr[j]-tempmean)*(1.0*arr[j]-tempmean);
				}
				int varlocalint = (int) varlocal;
				char * messvar = (char *) (&varlocalint);
				// printf(1,"child %d sending varlocal %d mess %s\n",i,varlocalint, messvar);
				stat=-1;
				while(stat!=0){
					stat=send(cid,masterpid,messvar);
				}
			}

			

			exit();
		}else{
			// cidarr[i] =cid;
			// printf(1,"Adding to cidarr ---- %d----\n",cid);
            if(type==1){

				cidarr[i] = cid;
				// printf(1, "Cidarr last element is ----- %d----\n",cidarr[i]);
				signaldone[i]=1;
			}
		}
	}
	for(int i=0;i<NUMTHREADS;i++){
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
	// printf(1,"Tot sum is %d\n",tot_sum);
	// int tempsum=0;
	// for(int i=0;i<1000;i++){
	// 	tempsum += arr[i];
	// }
	// printf(1,"correct sum is %d\n", tempsum);


	//VARIANCE PART
	if(type==1){

		while(1==1){
			int alldone=1;
			for(int i=0;i<NUMTHREADS;i++){
				if(signaldone[i]==0){
					alldone=0;
					break;
				}
			}
			if(alldone==1){
				break;
			}
		}
		sleep(100);

		// float mean = (1.0*tot_sum)/1000;
		char *summsg = (char *)malloc(MSGSIZE);
		summsg = (char *) &tot_sum;
		send_multi(masterpid,NUMTHREADS,cidarr,summsg);
		int tempvariance=0;
		for(int i=0;i<NUMTHREADS;i++){
			char *msg = (char *)malloc(MSGSIZE);
			int stat=-1;
			while(stat==-1){
				// printf(1,"looping while \n");
				stat = recv(msg);
			}

			int* tempvar = (int *) msg;
			// printf(1,"master iteration %d adding %d\n",i,*tempvar);
			// tot_sum += *temptotal;
			tempvariance=add(tempvariance, *tempvar);
			ansarr[i]=tempvariance;
			if(i>0){
				if(ansarr[i]==ansarr[i-1]){
					printf(1,"problem in %d iteration\n",i);
				}
			}
			// ansarr[i]=*temptotal;
		}
		variance = (1.0*tempvariance)/(1000);
		// while(1==1){

		// }
		// float temprealvar =0.0;
		// for(int i=0;i<1000;i++){
		// 	temprealvar += (1.0*(arr[i])- 4.545)*(1.0*(arr[i])- 4.545);
		// }
		// temprealvar = temprealvar/1000;
		// printf(1,"Real variance is ----- %d -------------\n", (int)temprealvar);
	}







  	//------------------

  	if(type==0){ //unicast sum
		printf(1,"Sum of array for file %s is %d\n", filename,tot_sum);
	}
	else{ //mulicast variance
		printf(1,"Variance of array for file %s is %d\n", filename,(int)variance);
	}
	exit();
}
