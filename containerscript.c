#include "types.h"
#include "stat.h"
#include "user.h"
int maxprocess=5;
void schedulercustom(void){
    int numprocess=2;
    int * processstate=(int *)malloc(maxprocess*sizeof(int));
    int * sleepschedule=(int *)malloc(maxprocess*sizeof(int));
    // int count=0;
    // int numprocess=0;
    // int * statusprocess=(int *)malloc(maxprocess*sizeof(int));
    // samplecall(&numprocess,&statusprocess);
    for(;;){
        registerState(&numprocess,processstate,sleepschedule);
        // printf(1,"scheduler\n");
        // while
        printf(1,"numprocess are %d\n",numprocess);
        for(int i=0;i<numprocess;i++){
            printf(1, "state %d sleep %d\n",processstate[i],sleepschedule[i]);
        }
        scheduler_call();
        // printf(1, "size is %d\n")
        // count++;
        // if(count>100000){
        //     ps();
        // }
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
    sleep(20);
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(1);
        // printf(1,"joined container\n");
        sleep(2);
        int count =0;
        for(;;){
            printf(1,"+");
            count++;
            if(count>500){
                ps();
                leave_container();
                exit();
            }
        }
    }
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(1);
        // printf(1,"joined container\n");
        sleep(2);
        int count =0;
        for(;;){
            printf(1,"-");
            count++;
            if(count>2000){
                leave_container();
                destroy_container(1);
                ps();
                sleep(200);
                ps();
                exit();
            }
        }
    }
    // for(;;){

    // }
    sleep(1000);
    exit();

}