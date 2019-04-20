#include "types.h"
#include "stat.h"
#include "user.h"
void schedulercustom(void){
    // int count=0;
    for(;;){
        // printf(1,"scheduler\n");
        scheduler_call();
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
            if(count>50){
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
            if(count>200){
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