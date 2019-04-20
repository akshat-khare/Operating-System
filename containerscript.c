#include "types.h"
#include "stat.h"
#include "user.h"
void schedulercustom(void){
    for(;;){
        // printf(1,"scheduler\n");
        scheduler_call();
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
        for(;;){
            printf(1,"+");
        }
    }
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(1);
        // printf(1,"joined container\n");
        for(;;){
            printf(1,"-");
        }
    }
    for(;;){
        
    }
    exit();

}