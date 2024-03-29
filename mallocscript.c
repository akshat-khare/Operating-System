#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
char bufcat[512];

void
cat(int fd)
{
  int n;

  while((n = read(fd, bufcat, sizeof(bufcat))) > 0) {
    if (write(1, bufcat, n) != n) {
      printf(1, "cat: write error\n");
      exit();
    }
  }
  if(n < 0){
    printf(1, "cat: read error\n");
    exit();
  }
}

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf(1,"this is file\n");
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    printf(1,"this is dir\n");
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
    //   if(fmtname(buf)[0]=='R'){
    //       continue;
    //   }
      printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}
int maxprocess=5;
#define HEAPSIZEMAX 300
// #define MAXSTRLEN 30
// #define MAXFILE 40
// char realname[MAXFILE][MAXSTRLEN];
// char virtualname[MAXFILE][MAXSTRLEN];

void schedulercustom(void){
    int numprocess=0;
    int * processstate=(int *)malloc(MAXUSERCONTAINERPROCESS*sizeof(int));
    int * mallocheap=(int *)malloc(sizeof(int)*HEAPSIZEMAX);
    int heapfreeptr=0;
    int * sleepschedule=(int *)malloc(MAXUSERCONTAINERPROCESS*sizeof(int));
    char * bufcharme =(char *)malloc(30*sizeof(char));
    int fdarg=-1;
    int count=0;
    int mallocaddr=0;
    int mallocbuf=0;
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
            int res = registerState(1,&numprocess,processstate,sleepschedule,&containerjustcalled, syscallping, &typesyscall,bufcharme,&fdarg, &mallocaddr, &mallocbuf);
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
            // printf(1,"syscall by %d of type %d\n",whichchildsyscalled, typesyscall);
            if(typesyscall==PS){
                //ps
                ps();
            }else if(typesyscall==CREATE){
                printf(1,"create encountered %s\n",bufcharme);

                fdarg=0;

            }else if(typesyscall==OPEN){
                printf(1,"open encountered %s\n",bufcharme);
                fdarg=0;


            }else if(typesyscall==WRITE){
                printf(1,"write encountered %s\n",bufcharme);

            }else if(typesyscall==CAT){
                printf(1,"cat encountered %s\n",bufcharme);
                int fdr = open(bufcharme,O_RDONLY);
                cat(fdr);
                
            }else if(typesyscall==LS){
                ls(".");
       
            }else if(typesyscall==MALLOC){
                // printf(1,"malloc encountered %d\n",mallocbuf);

                mallocaddr=heapfreeptr;
                heapfreeptr =heapfreeptr+mallocbuf;
            }else if(typesyscall==MALLOC_WRITE){
                // printf(1,"malloc write encountered %d %d\n",mallocaddr,mallocbuf);
                mallocheap[mallocaddr]=mallocbuf;
            }else if(typesyscall==MALLOC_READ){
                // printf(1,"malloc read encountered %d\n",mallocaddr);
                mallocbuf=mallocheap[mallocaddr];
            }
            syscallping[whichchildsyscalled]=2;
            typesyscall=-1;
            registerState(2,&numprocess,processstate,sleepschedule,&containerjustcalled, syscallping, &typesyscall,bufcharme,&fdarg,&mallocaddr,&mallocbuf);
   
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
            registerState(2,&numprocess,processstate,sleepschedule,&containerjustcalled, syscallping, &typesyscall, bufcharme,&fdarg,&mallocaddr,&mallocbuf);
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
    // pid =fork();
    // if(pid==0){
    //     create_container(numcontainer);
    //     schedulercustom();
    // }else{
    //     numcontainer++;
    // }
    // pid =fork();
    // if(pid==0){
    //     create_container(numcontainer);
    //     schedulercustom();
    // }else{
    //     numcontainer++;
    // }
    sleep(20);
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(1);
        // printf(1,"----------joined container-------\n");
        sleep(2);
        int waittemp=-1;
        // int count =0;
        // int fd=-1;
        // char * buftempproc=(char*)malloc(30*sizeof(char));
        // // char buftempproc[30];
        // strcpy(buftempproc,"README");
        // open("myfile",O_RDONLY);
        // write(0,buftempproc,30);
        // cat_sys(buftempproc);
        // ls_sys();
        int tempmalbuf=1;
        int tempmaladdr=-1;
        printf(1,"doing malloc asking for %d \n",tempmalbuf);
        malloc_sys(&tempmalbuf);
        while(waittemp==-1){
            waittemp=getStatusSysCall();
        }
        getmallocaddr_sys(&tempmaladdr);
        printf(1, "got address %d\n", tempmaladdr);

        tempmalbuf=4;
        printf(1, "writing on address %d value %d\n",tempmaladdr,tempmalbuf);
        writemalloc_sys(&tempmaladdr,&tempmalbuf);
        while(waittemp==-1){
            waittemp=getStatusSysCall();
        }
        printf(1, "reading from address %d\n",tempmaladdr);
        readmalloc_sys(&tempmaladdr);
        while(waittemp==-1){
            waittemp=getStatusSysCall();
        }
        readvalmalloc_sys(&tempmalbuf);
        printf(1, "value at address %d was %d\n",tempmaladdr, tempmalbuf);


        // int tempmal2=4;
        // readmalloc_sys(&tempmal);
        // printf(1," malloc read registered\n");
        // int waittemp = -1;
        // while(waittemp==-1){
        //     waittemp=getStatusSysCall();
        // }
        // readvalmalloc_sys(&tempmal);
        // printf(1,"done waiting for answer %d\n",tempmal);
        // getfd(&fd);
        // printf(1,"fd found %d\n",fd);
        // char mywrite[30];
        // strcpy(mywrite,"hi\0");
        // write(fd,&mywrite,sizeof(char)*30);
        // waittemp=-1;
        // while(waittemp==-1){
        //     waittemp=getStatusSysCall();
        // }
        // cat_sys("myfile");
        // waittemp=-1;
        // while(waittemp==-1){
        //     waittemp=getStatusSysCall();
        // }
        for(;;){
            // printf(1,"$1_1$\n");
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
    // pid = fork();
    // if(pid==0){
    //     // printf(1,"trying joining\n");
    //     join_container(1);
    //     // printf(1,"joined container\n");
    //     sleep(2);
    //     // int count =0;
    //     for(;;){
    //         printf(1,"$1_2$\n");
    //         // count++;
    //         // if(count>2000){
    //         //     leave_container();
    //         //     destroy_container(1);
    //         //     ps();
    //         //     sleep(200);
    //         //     ps();
    //         //     exit();
    //         // }
    //         // if(count%500==0){
    //         //     printf(1, "doing sys\n");
    //         // registerSysCall(1);
    //         // int waittemp = -1;
    //         // while(waittemp==-1){
    //         //     printf(1,".");
    //         //     waittemp=getStatusSysCall();
    //         // }
    //         // //     leave_container();
    //         // //     exit();
    //         // }
    //     }
    // }
    // pid = fork();
    // if(pid==0){
    //     // printf(1,"trying joining\n");
    //     join_container(1);
    //     // printf(1,"joined container\n");
    //     sleep(2);
    //     // int count =0;
    //     for(;;){
    //         printf(1,"$1_3$\n");
    //         // count++;
    //         // if(count%500==0){
    //         //     printf(1, "doing sys\n");
    //         //     registerSysCall(1);
    //         //     int waittemp = -1;
    //         //     while(waittemp==-1){
    //         //         printf(1,".");
    //         //         waittemp=getStatusSysCall();
    //         //     }
    //         // //     leave_container();
    //         // //     exit();
    //         // }
    //     }
    // }
    // pid = fork();
    // if(pid==0){
    //     // printf(1,"trying joining\n");
    //     join_container(2);
    //     // printf(1,"joined container\n");
    //     sleep(2);
    //     // int count =0;
    //     for(;;){
    //         printf(1,"$2_1$\n");
    //         // count++;
    //         // if(count%500==0){
    //         //     printf(1, "doing sys\n");
    //         //     registerSysCall(1);
    //         //     int waittemp = -1;
    //         //     while(waittemp==-1){
    //         //         printf(1,".");
    //         //         waittemp=getStatusSysCall();
    //         //     }
    //         // //     leave_container();
    //         // //     exit();
    //         // }
    //     }
    // }
    // pid = fork();
    // if(pid==0){
    //     // printf(1,"trying joining\n");
    //     join_container(3);
    //     // printf(1,"joined container\n");
    //     sleep(2);
    //     // int count =0;
    //     for(;;){
    //         printf(1,"$3_1$\n");
    //         // count++;
    //         // if(count%500==0){
    //         //     printf(1, "doing sys\n");
    //         //     registerSysCall(1);
    //         //     int waittemp = -1;
    //         //     while(waittemp==-1){
    //         //         printf(1,".");
    //         //         waittemp=getStatusSysCall();
    //         //     }
    //         // //     leave_container();
    //         // //     exit();
    //         // }
    //     }
    // }
    for(;;){

    }
    // sleep(1000);
    exit();

}