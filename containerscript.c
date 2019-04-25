#include "types.h"
#include "stat.h"
#include "user.h"
int maxprocess=5;
void schedulercustom(void){
    int numprocess=0;
    int * processstate=(int *)malloc(MAXUSERCONTAINERPROCESS*sizeof(int));
    int * sleepschedule=(int *)malloc(MAXUSERCONTAINERPROCESS*sizeof(int));
    int count=0;
    // int numprocess=0;
    // int * statusprocess=(int *)malloc(maxprocess*sizeof(int));
    // samplecall(&numprocess,&statusprocess);
    // int repeat=-1;
    int containerjustcalled=1;
    int * syscallping = (int *)malloc(MAXUSERCONTAINERPROCESS*sizeof(int));
    int typesyscall = -1;
    int issyscalldone=0;
    int whichchildsyscalled=-1;
    for(;;){
        count++;
        // ps();
        if(count%200==0){
            // printf(1,"&");
            // ps();
            int res = registerState(1,&numprocess,processstate,sleepschedule,&containerjustcalled, syscallping, &typesyscall);
            // printf(1,"^");
            if(res!=0){
                printf(1,"error");
            }
            count=0;
        }else{
            continue;
        }
        // printf(1,"scheduler\n");
        // while
        // printf(1,"numprocess are %d\n",numprocess);
        // for(int i=0;i<numprocess;i++){
        //     printf(1, "state %d sleep %d\n",processstate[i],sleepschedule[i]);
        // }
        issyscalldone=0;
        whichchildsyscalled=-1;
        for(int i=0;i<numprocess;i++){
            if(syscallping[i]==1){
                issyscalldone=1;
                whichchildsyscalled=i;
                break;
            }
        }
        if(issyscalldone==1){
            printf(1,"syscall by %d of type %d\n",whichchildsyscalled, typesyscall);
            if(typesyscall==1){
                //ps
                ps();
                syscallping[whichchildsyscalled]=2;
                typesyscall=-1;
                registerState(2,&numprocess,processstate,sleepschedule,&containerjustcalled, syscallping, &typesyscall);
            }
            // containerjustcalled=0;
        }else{
            if(containerjustcalled==0){
                // sleep(2);
                continue;
            }
            // printf(1,"*");
            if(numprocess==0){
                // do nothing maybe
            }else if(numprocess==1){
                if(processstate[0]==2 || sleepschedule[0]==1){
                    sleepschedule[0]=0;
                }
            }else{
                int startset=0;
                int schedset=0;
                int ptobeslept=-1;
                int ptobewaken=-1;
                // for (int i=0;i<numprocess;i++){
                //     if(startset==0){
                //         if((processstate[i]==3 || processstate[i]==4) && sleepschedule[i]==0){
                //             ptobeslept=i;
                //             startset=1;
                //         }
                //     }else if(startset==1){
                //         if(processstate[i]==2 || sleepschedule[i]==1){
                //             ptobewaken = i;
                //             schedset=1;
                //             break;
                //         } 
                //     }
                // }
                // if(startset==0){
                //     for(int i=0;i<numprocess;i++){
                //         if(processstate[i]==2 || sleepschedule[i]==1){
                //             ptobewaken=i;
                //             schedset=1;
                //             break;
                //         }
                //     }
                // }
                // if(schedset==0){
                //     sleepschedule[ptobeslept]=1;
                // }
                // if(schedset==1){
                //     sleepschedule[ptobewaken]=0;
                //     if(repeat==ptobeslept){
                //         if(startset==0){

                //             for(int i=0;i<numprocess;i++){
                //                 sleepschedule[i]=0;
                //             }
                //         }
                //     }
                //     repeat=ptobeslept;
                //     if(startset==1 && processstate[ptobeslept]!=4){
                //         sleepschedule[ptobeslept]=1;
                //     }

                // }

                for (int i=0;i<numprocess;i++){
                    if(startset==0){
                        if((processstate[i]==3 || processstate[i]==4) && sleepschedule[i]==0){
                            ptobeslept=i;
                            startset=1;
                        }
                    }else if(startset==1){
                        if(processstate[i]==2 || sleepschedule[i]==1){
                            ptobewaken = i;
                            schedset=1;
                            break;
                        } 
                    }
                }
                if(startset==1 && schedset==0){
                    if(ptobeslept==numprocess-1){
                        if(processstate[0]==2 || sleepschedule[0]==1){

                            ptobewaken=0;
                            schedset=1;
                        }else{
                            //idk
                        }
                    }else{
                        //just sleep the waken up
                    }
                }
                if(startset==0){
                    ptobewaken=0;
                    schedset=1;
                }
                if(schedset==1){
                    sleepschedule[ptobewaken]=0;
                }
                if(startset==1){
                    sleepschedule[ptobeslept]=1;
                    // repeat = ptobeslept;
                }


            }

            // scheduler_call();
            // printf(1, "size is %d\n")
            // count++;
            // if(count>100000){
            //     ps();
            // }
            // sleepschedule[0]=1;
            // sleepschedule[1]=0;
            containerjustcalled=0;
            // printf(1,"decided\n");
            // for(int i=0;i<numprocess;i++){
            //     printf(1,"%d ",sleepschedule[i]);
            // }
            // printf(1,"\n");
            registerState(2,&numprocess,processstate,sleepschedule,&containerjustcalled, syscallping, &typesyscall);
        }

    }
}

int main(void){
    int pid;
    int numcontainer=1;
    pid =fork();
    if(pid==0){
        create_container(numcontainer);
        schedulercustom();
    }else{
        numcontainer++;

    }
    pid =fork();
    if(pid==0){
        create_container(numcontainer);
        schedulercustom();
    }else{
        numcontainer++;
    }
    pid =fork();
    if(pid==0){
        create_container(numcontainer);
        schedulercustom();
    }else{
        numcontainer++;
    }
    sleep(20);
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(1);
        // printf(1,"joined container\n");
        sleep(2);
        // int count =0;
        for(;;){
            printf(1,"$1_1$\n");
            // count++;
            // if(count%500==0){
            //     printf(1, "doing sys\n");
            //     registerSysCall(1);
            //     int waittemp = -1;
            //     while(waittemp==-1){
            //         printf(1,".");
            //         waittemp=getStatusSysCall();
            //     }
            // //     leave_container();
            // //     exit();
            // }
        }
    }
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(1);
        // printf(1,"joined container\n");
        sleep(2);
        // int count =0;
        for(;;){
            printf(1,"$1_2$\n");
            // count++;
            // if(count>2000){
            //     leave_container();
            //     destroy_container(1);
            //     ps();
            //     sleep(200);
            //     ps();
            //     exit();
            // }
            // if(count%500==0){
            //     printf(1, "doing sys\n");
            //     registerSysCall(1);
            //     int waittemp = -1;
            //     while(waittemp==-1){
            //         printf(1,".");
            //         waittemp=getStatusSysCall();
            //     }
            // //     leave_container();
            // //     exit();
            // }
        }
    }
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(1);
        // printf(1,"joined container\n");
        sleep(2);
        // int count =0;
        for(;;){
            printf(1,"$1_3$\n");
            // count++;
            // if(count%500==0){
            //     printf(1, "doing sys\n");
            //     registerSysCall(1);
            //     int waittemp = -1;
            //     while(waittemp==-1){
            //         printf(1,".");
            //         waittemp=getStatusSysCall();
            //     }
            // //     leave_container();
            // //     exit();
            // }
        }
    }
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(2);
        // printf(1,"joined container\n");
        sleep(2);
        // int count =0;
        for(;;){
            printf(1,"$2_1$\n");
            // count++;
            // if(count%500==0){
            //     printf(1, "doing sys\n");
            //     registerSysCall(1);
            //     int waittemp = -1;
            //     while(waittemp==-1){
            //         printf(1,".");
            //         waittemp=getStatusSysCall();
            //     }
            // //     leave_container();
            // //     exit();
            // }
        }
    }
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(3);
        // printf(1,"joined container\n");
        sleep(2);
        // int count =0;
        for(;;){
            printf(1,"$3_1$\n");
            // count++;
            // if(count%500==0){
            //     printf(1, "doing sys\n");
            //     registerSysCall(1);
            //     int waittemp = -1;
            //     while(waittemp==-1){
            //         printf(1,".");
            //         waittemp=getStatusSysCall();
            //     }
            // //     leave_container();
            // //     exit();
            // }
        }
    }
    for(;;){

    }
    // sleep(1000);
    exit();

}