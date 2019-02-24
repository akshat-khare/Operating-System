#include "types.h"
#include "stat.h"
#include "user.h"
#define NUMCHILD 8
int cidarr[NUMCHILD];
// int cidarrsister[NUMCHILD];
int brother=-1;
// int sister=-1;
void handlesigint(int sinum){
    // printf(1, "Awakened\n");
    char *msgmulti = (char *)malloc(MSGSIZE);
    recvmulti(msgmulti);
    printf(1,"Message is %s\n",msgmulti);
    // int brother=0;
    int pid=getpid();
    // int temp=0;
    // for(int i=0;i<NUMCHILD;i++){
    //     printf(1,"brother sister----%d %d---\n", cidarr[i],cidarrsister[i]);
    // }
    // for(int i=0;i<NUMCHILD;i++){
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
    // for(int i=0;i<NUMCHILD;i++){
    //     printf(1,"brother sister----%d %d---\n", cidarr[i],cidarrsister[i]);
    // }
    int pid = getpid();
    int cid=fork();
    if(cid==0){
        int myrealpid = getpid();
        // for(int i=0;i<NUMCHILD;i++){
        //     if(cidarr[i]==pid){
        //         cidarrsister[i]=myrealpid;
        //         break;
        //     }
        // }
        brother=pid;
        // for(int i=0;i<NUMCHILD;i++){
        //     printf(1,"brother sister----%d %d---\n", cidarr[i],cidarrsister[i]);
        // }
        // printf(1,"Calling register handler for pid, cid %d %d\n",pid,myrealpid);
        registerhandler(pid,myrealpid);
        // printf(1,"After registerhandler for pid, cid %d %d\n",pid,myrealpid);
        handler(typesig);
    }else{
        // printf(1,"Signal registering for pid, cid %d, %d\n",pid,cid);
        // int temp=0;
        // for(int i=0;i<NUMCHILD;i++){
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
    // int NUMCHILD=4;
    int signaldone[NUMCHILD];
    for(int i=0;i<NUMCHILD;i++){
        signaldone[i]=0;
        // cidarr[i]=-1;
        // cidarrsister[i]=-1;
    }
    int masterpid = getpid();
    int cidarr[NUMCHILD];
    // int* cidarr = (int *)malloc(NUMCHILD);
    for(int i=0;i<NUMCHILD;i++){
        cidarr[i]=-2;
    }
	for(int i=0;i<NUMCHILD;i++){
        int cid = fork();
        if(cid==0){
            cidarr[i]=getpid();
            brother=getpid();
            // for(int i=0;i<NUMCHILD;i++){
            //     printf(1,"brother sister----%d %d---\n", cidarr[i],cidarrsister[i]);
            // }
            signal(1,handlesigint);
            // cidarr[i] = getpid();
            char *msgbrother = (char *)malloc(MSGSIZE);
            int stat=-1;
            while(stat==-1){
            // signaldone[i]=1;
                stat=recv(msgbrother);
            }
            printf(1,"Finmessage is %s\n", msgbrother);
            exit();
        }else{
            // printf(1,"Adding to cidarr ---- %d----\n",cid);
            cidarr[i] = cid;
            // printf(1, "Cidarr last element is ----- %d----\n",cidarr[i]);
            signaldone[i]=1;
        }
    }
    // for(int i=0;i<NUMCHILD;i++){
    //     printf(1, "Cidarr last element is ----- %d----\n",cidarr[i]);
    // }
    while(1==1){
        int alldone=1;
        for(int i=0;i<NUMCHILD;i++){
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
    // ps();
    // for(int i=0;i<NUMCHILD;i++){
    //     printf(1, "Cidarr last element is -----%d----\n",cidarr[i]);
    // }
    // printf(1,"Done waiting for all threads to declare their signals\n");
    char *msg = (char *)malloc(MSGSIZE);
	msg = "Hi";
    // printf(1,"Cidarr is \n");
    // for(int i=0;i<NUMCHILD;i++){
    //     printf(1,"--%d %d--- \n",i,cidarr[i]);
    // }
    // printf(1," End of cidarr\n");
    // for(int i=0;i<NUMCHILD;i++){
    //     printf(1,"brother sister----%d %d---\n", cidarr[i],cidarrsister[i]);
    // }
    send_multi(masterpid,NUMCHILD, cidarr,(void *) msg);
    // ps();
    while(1==1){

    }
	exit();
}
