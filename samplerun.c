#include "types.h"
#include "stat.h"
#include "user.h"
int main(int argc, char *argv[]){
    // printf(1,"samplerun called %s\n",argv[1]);
    // if(atoi(argv[1])==1){
    //     // printf(1,"lol\n");
    //     exit();
    // }
    // // if(argv)
    // // samplecall();
    // // printf(1,"hi\n");
    // // printf(1,"malloc1\n");
    // // int temp = (int)malloc(sizeof(int));
    // // // int temp;
    // // temp=4;
    // // // int * temparr = (int *)
    // // printf(1,"malloc2\n");
    // // int temp1 = (int)malloc(sizeof(int));
    // // // int temp1;
    // // temp1=5;
    // // printf(1,"samplerun %p\n",(int *) &temp);
    // // printf(1,"samplerun %p\n",(int *) &temp1);
    // // printf(1,"%d\n",temp+temp1);
    // printf(1,"calling samplerun\n");
    // char*argcus[] = {"samplerun","1"};
    // int pid =fork();
    // if(pid==0){
    // exec("samplerun",argcus);

    // }else{

    // printf(1,"was this line executed\n");
    // exit();
    // }
    // printf(1,"size is %d\n",sizeof(int));
    // int sampleint =4;
    // int * samplearr = (int *)malloc(4*sizeof(int));
    // samplearr[0]=1;
    // samplearr[1]=2;
    // samplearr[2]=3;
    // samplearr[3]=4;
    // samplecall(&sampleint,samplearr);
    // // printf(1, "user numer is %d\n", sampleint);
    // printf(1,"user arr ele is %d\n",samplearr[3]);
    // samplearr[3]=7;
    // samplecall(&sampleint,samplearr);
    // printf(1,"user arr ele is %d\n",samplearr[3]);
    int sampleint=4;
    samplecall(&sampleint);
    printf(1,"%d num\n",sampleint);

    exit();

}