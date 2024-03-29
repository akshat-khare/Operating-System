#include "types.h"
#include "stat.h"
#include "user.h"
// #include <stdio.h> 
// #include <stdlib.h>
// #include <sys/types.h> 
// #include <unistd.h> 
// #include<string.h> 
// #include<sys/wait.h> 
// #include<time.h>
// higher thread has higher priority
// #define P 9
// #define Psq 3
// #define P1 0
// #define P2 3
// #define P3 6
#define REQUEST 1
#define REPLY 2
#define RELEASE 3
#define GRANT 4
#define INQUIRE 5
#define FAILED 6
#define YIELD 7
#define KILLCHILD 8
float fabsm(float a){
	if(a<0)
	return -1*a;
return a;
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
int mysqrt(int P){
    int i=0;
    for(i=0;i<P;i++){
        if(i*i==P){
            break;
        }
    }
    return i;
}
int main(int argc, char *argv[])
{
    int P=0;
    int Psq=0;
    int P1=0;
    int P2=0;
    int P3=0;
    char * filename;
    filename = "assig2b.inp";
    int fdfile = open(filename,0);
    P = readint(fdfile);
    P1 = readint(fdfile);
    P2 = readint(fdfile);
    P3 = readint(fdfile);
    if(P3==-1){
        printf(1,"error\n");
    }
    Psq = mysqrt(P);
    close(fdfile);
    int i,j;
    int numrow,numcol;
    //every pipe is incoming
    int ** fdpipes = (int**)malloc(sizeof(int*)*(P));
    float messagebuffer;
    int sender;
	for(i=0;i<P;i++){
		fdpipes[i] = (int *)malloc(sizeof(int)*2);
	}
	for(i=0;i<P;i++){
		if (pipe(fdpipes[i])==-1) 
		{ 
			printf(1,"Pipe Failed\n" ); 
			return 1; 
    	} 
	}
    int ** fdmasterpipes = (int**)malloc(sizeof(int*)*(P)*2);
    for(i=0;i<2*P;i++){
		fdmasterpipes[i] = (int *)malloc(sizeof(int)*2);
	}
	for(i=0;i<2*P;i++){
		if (pipe(fdmasterpipes[i])==-1) 
		{ 
			printf(1,"Pipe Failed\n" ); 
			return 1; 
    	} 
	}
    int gridhelper[Psq][Psq];
    for(i=0;i<Psq;i++){
        for(j=0;j<Psq;j++){
            gridhelper[i][j] = Psq*i+j;
        }
    }
    //-1 in request queue means absence 1 means presence
    int requestqueue[P];
    int inquirequeue[P];
    int topper;
    for(i=0;i<P;i++){
        requestqueue[i]=-1;
        inquirequeue[i]=-1;
    }
    int grantsecuredarr[P];
    int numgrantssecured=0;
    int numfailedreq=0;
    for(i=0;i<P;i++){
        grantsecuredarr[i]=-1;
    }
    //1 means I have the lock -1 means I don't have the lock or grant
    int grantlock=1;
    //-1 grantgiven to means grant given to none
    int grantgivento=-1;
    int tid=0;
    int targettid;
    // int masterpid = getpid();
    int childpid=0;
    // int childpidarr[P];
    int ptype=0;
    int mybool;
    // printf(1,"Grid is\n");
    // for(i=0;i<Psq;i++){
    //     for(j=0;j<Psq;j++){
    //         printf(1,"%d ",gridhelper[i][j]);
    //     }
    //     printf(1,"\n");
    // }
    for(i=0;i<P;i++){
        childpid=fork();
        if(childpid<0){
            printf(1,"Fork failed\n");
        }else if(childpid>0){
            // childpidarr[i]=childpid;
        }else{
            tid=i;
            if(i<P1){
                ptype=1;
            }else if(i<P1+P2){
                ptype=2;
            }else{
                ptype=3;
            }
            break;
        }
    }
    if(childpid==0){
        // printf(1,"%d child starting of type %d\n",tid,ptype);
        if(ptype!=1){
            // depending on ptype 2 or 3 i either sleep or not after acquring lock
            //acquire the lock
            messagebuffer = 1.0*REQUEST + 10.0*tid;
            numrow = tid/Psq;
            numcol = tid%Psq;
            // printf(1,"numrow is %d numcol is %d\n",numrow,numcol);
            for(i=0;i<Psq;i++){
                if(i==numrow){
                    continue;
                }
                messagebuffer = 1.0*REQUEST + 10.0*tid;
                // printf(1,"Grid is\n");
                // for(i=0;i<Psq;i++){
                //     for(j=0;j<Psq;j++){
                //         printf(1,"%d ",gridhelper[i][j]);
                //     }
                //     printf(1,"\n");
                // }
                // printf(1,"i is %d numcol is %d\n",i,numcol);
                targettid = gridhelper[i][numcol];
                write(fdpipes[targettid][1],&messagebuffer,sizeof(float));
                // printf(1,"%d has sent request signals to %d\n",tid,targettid);
                
            }
            for(i=0;i<Psq;i++){
                if(i==numcol){
                    continue;
                }
                messagebuffer = 1.0*REQUEST + 10.0*tid;
                // printf(1,"Grid is\n");
                // for(i=0;i<Psq;i++){
                //     for(j=0;j<Psq;j++){
                //         printf(1,"%d ",gridhelper[i][j]);
                //     }
                //     printf(1,"\n");
                // }
                targettid = gridhelper[numrow][i];
                write(fdpipes[targettid][1],&messagebuffer,sizeof(float));
                // printf(1,"%d has sent request signals to %d\n",tid,targettid);
            }
            // printf(1,"This must be print once for %d once\n",tid);
            messagebuffer = 1.0*REQUEST + 10.0*tid;
            write(fdpipes[tid][1],&messagebuffer,sizeof(float));
            // printf(1,"%d has sent request signals to %d\n",tid,tid);
            //sent the request to acquire lock

        }
        

        for(;;){
            read(fdpipes[tid][0],&messagebuffer,sizeof(float));
            int typemessage = ((int) messagebuffer)%10;
            sender = (int) (messagebuffer/10);
            // printf(1,"%d read message of type %d from sender %d\n",tid,typemessage,sender);
            if(typemessage==REQUEST){
                sender = (int) (messagebuffer/10);
                if(grantlock==1){
                    //I have the lock I can grant the lock
                    grantlock=-1;
                    messagebuffer=1.0*GRANT + 10.0*tid;
                    grantgivento=sender;
                    requestqueue[sender]=1;
                    write(fdpipes[sender][1],&messagebuffer,sizeof(float));

                }else{
                    //if i dont have the lock
                    //queue the sender
                    requestqueue[sender]=1;
                    //priority based case now
                    if(grantgivento>=sender){
                        //send failed
                        messagebuffer = 1.0*FAILED + 10.0*tid;
                        write(fdpipes[sender][1],&messagebuffer,sizeof(float));

                    }else if(grantgivento<sender)
                    {
                        //inquire the grantgiven
                        messagebuffer = 1.0*INQUIRE + 10.0*tid;
                        // printf(1,"%d is sending inquire to %d for sender %d\n",tid, grantgivento, sender);
                        write(fdpipes[grantgivento][1],&messagebuffer,sizeof(float));
                    }else{
                        printf(1,"can't happen\n");
                    }
                    
                }
            }else if(typemessage==RELEASE){
                sender = (int) (messagebuffer/10);
                topper=-1;
                requestqueue[sender]=-1;
                for(i=P-1;i>=0;i--){
                    if(requestqueue[i]==-1){
                        continue;
                    }else if(requestqueue[i]==1){
                        topper=i;
                        break;
                    }
                }
                // printf(1,"topper is %d\n",topper);
                if(topper==-1){
                    // printf(1,"fishy behaviour in release\n");
                    grantgivento=-1;
                    grantlock=1;
                }else{
                    
                    grantgivento=topper;
                    grantlock = -1;
                    messagebuffer=1.0*GRANT+10.0*tid;
                    write(fdpipes[topper][1],&messagebuffer,sizeof(float));
                }
            }else if(typemessage==GRANT){
                sender = (int) (messagebuffer/10);
                grantsecuredarr[sender]=1;
                numgrantssecured+=1;

                
                // printf(1,"%d grantarr is \n",tid);
                // for(i=0;i<P;i++){
                //     printf(1,"%d ",grantsecuredarr[i]);
                // }
                // printf(1,"\n");
            }else if(typemessage==INQUIRE){
                // if(numfailedreq>0){
                //     // I should send yield
                //     sender = (int) (messagebuffer/10);
                //     numgrantssecured-=1;
                //     grantsecuredarr[sender] = -1;
                //     messagebuffer = 1.0*YIELD + 10.0*tid;
                //     write(fdpipes[sender][1],&messagebuffer, sizeof(float));
                // }
                mybool=1;
                numrow=tid/Psq;
                numcol=tid%Psq;
                for(i=0;i<Psq;i++){
                    if(grantsecuredarr[gridhelper[i][numcol]]==0){
                        mybool=0;
                        break;
                    }
                    if(grantsecuredarr[gridhelper[numrow][i]]==0){
                        mybool=0;
                        break;
                    }
                }
                sender = (int) (messagebuffer/10);
                if(mybool==0){
                    numgrantssecured-=1;
                    grantsecuredarr[sender] = -1;
                    messagebuffer = 1.0*YIELD + 10.0*tid;
                    write(fdpipes[sender][1],&messagebuffer, sizeof(float));
                }else{
                    // printf(1,"%d deffered inquire for %d\n",tid,sender);
                    inquirequeue[sender]=1;
                }
            }else if(typemessage==FAILED){
                sender = (int) (messagebuffer/10);
                numfailedreq+=1;
                grantsecuredarr[sender] = 0;
                // mybool=1;
                for(i=P-1;i>=0;i--){
                    if((grantsecuredarr[i]==1) && (inquirequeue[i]==1)){
                        grantsecuredarr[i]=-1;
                        inquirequeue[i]=-1;
                        messagebuffer = 1.0*YIELD + 10.0*tid;
                        write(fdpipes[sender][1],&messagebuffer, sizeof(float));
                    }
                }
            }else if(typemessage==YIELD){
                sender = (int) (messagebuffer/10);
                topper=-1;
                for(i=P-1;i>=0;i--){
                    if(requestqueue[i]==-1){
                        continue;
                    }else if(requestqueue[i]==1){
                        topper=i;
                        break;
                    }
                }
                if(topper==-1){
                    printf(1,"fishy behaviour in yield\n");
                }
                grantgivento=topper;
                grantlock = -1;
                messagebuffer=1.0*GRANT+10.0*tid;
                requestqueue[sender] = 1;
                write(fdpipes[topper][1],&messagebuffer,sizeof(float));

            }else if(typemessage==KILLCHILD){
                //kill myself
                exit();
            }else{
                printf(1,"unknown message pls help\n");
            }

            


            //check grants
            
            numrow = tid/Psq;
            numcol = tid%Psq;
            mybool=1;//true
            for(i=0;i<Psq;i++){
                if(grantsecuredarr[gridhelper[i][numcol]]!=1){
                    mybool=0;
                    break;
                }
                if(grantsecuredarr[gridhelper[numrow][i]]!=1){
                    mybool=0;
                    break;
                }
            }
            if(mybool==1){

            }else{
                for(i=P-1;i>=0;i--){
                    if((grantsecuredarr[i]==1) && (inquirequeue[i]==1)){
                        grantsecuredarr[i]=-1;
                        inquirequeue[i]=-1;
                        messagebuffer = 1.0*YIELD + 10.0*tid;
                        write(fdpipes[sender][1],&messagebuffer, sizeof(float));
                    }
                }
                mybool=0;
            }
            if(mybool==1){
                //I have all the grants
                printf(1,"%d acquired the lock at time %d.\n",getpid(), uptime());
                if(ptype==2){
                    sleep(200);
                }
                printf(1,"%d released the lock at time %d.\n",getpid(), uptime());
                if(ptype==1){
                    printf(1,"fishy yes it is as type 1 has lock lol\n");
                }
                //release the lock
                numgrantssecured=0;
                for(i=0;i<P;i++){
                    grantsecuredarr[i]=-1;
                }
                for(i=0;i<Psq;i++){
                    if(i==numrow){
                        continue;
                    }
                    messagebuffer=1.0*RELEASE+10.0*tid;
                    write(fdpipes[gridhelper[i][numcol]][1], &messagebuffer,sizeof(float));
                    // printf(1,"%d has sent release signals to %d\n",tid,gridhelper[i][numcol]);
                }
                for(i=0;i<Psq;i++){
                    if(i==numcol){
                        continue;
                    }
                    messagebuffer=1.0*RELEASE+10.0*tid;
                    write(fdpipes[gridhelper[numrow][i]][1], &messagebuffer,sizeof(float));
                    // printf(1,"%d has sent release signals to %d\n",tid,gridhelper[numrow][i]);

                }
                messagebuffer=1.0*RELEASE+10.0*tid;
                write(fdpipes[tid][1],&messagebuffer, sizeof(messagebuffer));
                // printf(1,"%d has sent release signals to %d\n",tid,tid);
                messagebuffer=-1.0;
                write(fdmasterpipes[2*tid+1][1], &messagebuffer,sizeof(messagebuffer));
            }

            
        }

        
    }else{
        // int count=0;
        for(i=0;i<P;i++){
            if(i<P1){
                continue;
            }
            read(fdmasterpipes[2*i+1][0],&messagebuffer,sizeof(messagebuffer));
        }
        // printf(1,"master declared all locks done and dusted\n");
        for(i=0;i<P;i++){
            messagebuffer=1.0*KILLCHILD;
            write(fdpipes[i][1],&messagebuffer,sizeof(messagebuffer));
        }
        // printf(1,"master sent all kill signals\n");
        for(i=0;i<P;i++){
            wait();
        }
        exit();
    }



}