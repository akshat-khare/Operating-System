#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_getcid(void)
{
  return myproc()->containerindex;
}

int
sys_sbrk(void)
{
  // cprintf("sbrk called\n");
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// int hii=0;
int
sys_halt(void)
{
  outb(0xf4, 0x00);
  // hii = hii +1;
  // cprintf("%d\n",hii);
  // cprintf("%d", syscallctr)
  return 0;
}
int 
sys_toggle(void){
  togglestate=1-togglestate;
  reinitializeprinthelper();
  return 0;
}
int
sys_print_count(void){
  printcounthelper();
  return 0;
}
int sys_add(void){
  int a;
  int b;
  argint(0, &a);
  argint(1, &b);
  // cprintf("a is %d\n",a);
  // cprintf("b is %d\n",b);
  int temp = a+b;
  // cprintf("%d\n",temp);
  return temp;
}
int sys_ps(void){
  printrunningprocess();
  return 0;
}
int sys_send(void){
  acquiresendlock();
  int c;
  int a;
  char * b;
  argint(0,&c);
  argint(1,&a);
  argstr(2,&b);
  // cprintf("a is %d\n",a);
  // cprintf("b is %s\n",b);
  // cprintf("freemessagebuffer is %d\n",free_message_buffer);
  int tempmsgbfr = getMessageBuffer();
  // cprintf("buffer got is %d\n",tempmsgbfr);
  // cprintf("freemessagebuffer is %d\n",free_message_buffer);

  // int * tempintptr = (int *) b;
  // cprintf("temp is %d\n",tempintptr[0]);
  // cprintf("len 1 is %d len 2 is %d\n", NELEM(b), NELEM(tempintptr));
  for(int i=1;i<MESSAGESIZE;i++){
    int temp = (int) (b[i-1]);
    message_buffer[tempmsgbfr][i]=temp;
  }
  // cprintf("message in send message is \n");
  // for(int i=1;i<MESSAGESIZE;i++){
  //   cprintf("%d",(message_buffer[tempmsgbfr][i]));
  // }
  // cprintf("\n");
  // cprintf("it actually means\n");
  // for(int i=1;i<MESSAGESIZE;i++){
  //   char temp =(char) message_buffer[tempmsgbfr][i]+'0';
  //   cprintf("%s",&temp);
  // }
  // cprintf("\n");
  pushmessage(a,tempmsgbfr);
  wakeupcustom(a);
  releasesendlock();
  return 0;
}
int sys_recv(void){
  int a;
  char* b;
  // argint(0,&a);
  a = myproc()->pid;
  argptr(0,&b, 2);
  // cprintf("executing pop message\n");
  // acquiresendlock();

  int temp = popmessage(a);
  // releasesendlock();
  // cprintf("temp of popmessage is %d\n",temp);
  // if(temp==-1){
    
  //   sleepcustom();
  //   return -1;
  // }
  // cprintf("recv temp is %d\n",temp);
  while(temp==-1){
    // cprintf("next instruction is sleep with temp %d\n",temp);
    sleepcustom();
    // cprintf("this was executed after sleep with temp %d\n",temp);
    temp=popmessage(a);
  }
  // cprintf("now copying from buffer %d\n",temp);

  for(int i=1;i<MESSAGESIZE;i++){
    b[i-1]=(char) ((message_buffer[temp])[i]);
  }
  // cprintf("freemessagebuffer is %d\n",free_message_buffer);
  // acquiresendlock();
  freeMessageBuffer(temp);
  // releasesendlock();
  // cprintf("freemessagebuffer is %d\n",free_message_buffer);

  // b[0]='a';
  // cprintf("recv part %s\n",b);
  return 0;
}
int sigchild[NPROC];
MessageBuffer multibuffer;
int sys_registerhandler(void){
  int a;
  int b;
  argint(0,&a);
  argint(1,&b);
  sigchild[a]=b;
  // cprintf("Going to sleep pid, cid %d %d\n", a, b);
  sleepcustom();
  return 0;
}
int sys_send_multi(void){
  int a;
  int b;
  int c;
  char * d;
  argint(0,&a);
  argint(1,&b);
  argint(2,&c);
  argstr(3,&d);
  int * e;
  e=(int *)c;
  int arr[b];
  for(int i=0;i<b;i++){
    // cprintf("adding to arr %d\n",*e);
    arr[i]=*e;
    e++;
  }
  for(int i=1;i<MESSAGESIZE;i++){
    int temp = (int) (d[i-1]);
    multibuffer[i]=temp;
  }
  for(int i=0;i<b;i++){
    // cprintf("waking up fork of %d that is %d\n",arr[i], sigchild[arr[i]]);
    wakeupcustom(sigchild[arr[i]]);
  }
  return 0;

}
int sys_recvmulti(void){
  char* b;
  argptr(0,&b, 2);
  for(int i=1;i<MESSAGESIZE;i++){
    b[i-1]=(char) (multibuffer[i]);
  }
  return 0;
}
extern struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;
// int samplevar[1];
// int * temp;

int sys_samplecall(void){
  // cprintf("been called\n");
  // boolwritewalkpage=1;
  // int 
  // int * temp;
  // cprintf("kerner temp initially %d\n",temp[0]);
  // cprintf("kernel %d\n",samplevar[0]);
  // argptr(1,(char **) &temp,4);
  // cprintf("kerner temp %d\n",temp[3]);
  // // samplevar[0]=temp[0];
  // // cprintf("kernel %d\n",samplevar[0]);
  // temp[3]=5;
  // samplevar[0]=5;
  // int *temp;
  // argptr(0,(char**)&temp,4);
  // *temp=5;
  int *temp;
  argptr(0,(char**) &temp,4);
  *temp=5;
  struct {
    char nma[2];
  }strme;
  argptr(0, (char **) &strme,*temp);
  cprintf("found\n");
  cprintf("%s\n",strme.nma);
  return 0;
}
int sys_create_container(void){
  myproc()->iscontainer=1;
  int a;
  argint(0,&a);
  myproc()->containerindex=a;
  myproc()->numprocess=0;
  myproc()->sleepschduled=0;
  myproc()->isassignedcontainer=-1;
  myproc()->containerassigned=-1;
  myproc()->containerjustcalled=1;
  return 0;
}
int sys_join_container(void){
  myproc()->isassignedcontainer=1;
  int a;
  argint(0,&a);
  myproc()->containerassigned=a;
  myproc()->iscontainer=-1;
  myproc()->containerindex=-1;
  myproc()->containerjustcalled=1;
  struct proc *p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->iscontainer == 1){
      if(p->containerindex==a){
        p->numprocess = p->numprocess+1;
        break;
      }
    }
  }
  if(p==&ptable.proc[NPROC]){
    cprintf("found no container to join to\n");
    return -1;
  }
  myproc()->sleepschduled=1;
  myproc()->hasdonesyscall=0;
  myproc()->typesyscall=0;
  myproc()->isSysCallComplete=0;
  // sleepcustom();
  // cprintf("pcontainerindex is %d\n",myproc()->containerassigned);
  // cprintf("numprocess is %d\n",*(numprocess[0]));
  // *(numprocess[myproc()->containerassigned-1])= *(numprocess[myproc()->containerassigned-1])+1;
  // cprintf("numprocess is %d\n",*(numprocess[myproc()->containerassigned-1]));

  return 0;
}
int repeat;
int sys_scheduler_call(void){
  // cprintf("scheduling\n");
  if(myproc()->iscontainer!=1){
    // cprintf("not a container\n");
    return -1;
  }
  if(myproc()->containerjustcalled==0){
    return -1;
  }
  struct proc *p;
  int containerindex=myproc()->containerindex;
  int tempnumprocess=myproc()->numprocess;
  if(tempnumprocess==0){
    return -1;
  }else if (tempnumprocess==1)
  {
    // cprintf("single process\n");
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->isassignedcontainer == 1){
        if(p->containerassigned ==containerindex){

          if(p->state==SLEEPING || p->sleepschduled==1){
            // cprintf("waking single up %d whose state is %d\n",p->pid, p->state);
            p->sleepschduled=0;
            // if(p->state==SLEEPING){
            //   release(&ptable.lock);
            //   wakeup(p);
            //   acquire(&ptable.lock);
            // }
            // cprintf("after waking up pid %d state is %d\n",p->pid,p->state);
            break;
          }
        }
      }
    }
    myproc()->containerjustcalled=0;
    release(&ptable.lock);
    return 0;
  }else{
    // cprintf("%d processes\n",tempnumprocess);
    acquire(&ptable.lock);
    // cprintf("sched with containerjustcalled %d\n",myproc()->containerjustcalled);
    int startset=0;
    int schedset=0;
    struct proc *p1=ptable.proc; //to be slept
    struct proc *p2=ptable.proc;// to be waken up
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->isassignedcontainer == 1){
        if(p->containerassigned ==containerindex){
          // cprintf("pid %d state %d my state %d sleep schedule %d my sleep sch %d\n",p->pid,p->state, myproc()->state, p->sleepschduled, myproc()->sleepschduled);
          if(startset==0){
            if((p->state==RUNNABLE || p->state==RUNNING) && p->sleepschduled==0){
              p1 = p;
              startset=1;
            }
          }else if(startset==1){
            if(p->state==SLEEPING || p->sleepschduled==1){
              p2=p;
              schedset=1;
              break;
            }
          }
        }
      }
    }
    if(startset==0 || schedset==0){
      for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
        if(p->isassignedcontainer == 1){
          if(p->containerassigned ==containerindex){
            if(p->state==SLEEPING || p->sleepschduled==1){
              p2=p;
              schedset=1;
              break;
            }
          }
        }
      }
    }
    if(schedset==0){
      // cprintf("gotta sleep one\n");
      p1->sleepschduled=1;
      release(&ptable.lock);
      return -1;
    }else{
      // cprintf("waking up %d whose state is %d\n",p2->pid, p2->state);
      p2->sleepschduled=0;
      if(repeat==p2->pid){
        if(startset==0 || schedset==0){
          for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
            if(p->isassignedcontainer == 1){
              if(p->containerassigned ==containerindex){
                p->sleepschduled=0;
              }
            }
          }
        }
      }
      repeat=p2->pid;
      // if(p2->state==SLEEPING){
      //   cprintf("need to wake up\n");
      //   release(&ptable.lock);
      //   wakeup(p2);
      //   // p2->state=RUNNABLE;
      //   acquire(&ptable.lock);
      // }
      // p2->state=RUNNABLE;
      // cprintf("after waking up pid %d state is %d\n",p->pid,p->state);
      if(startset==1 && p1->pid!=myproc()->pid && p1->state!=RUNNING){
    // cprintf("Pid: %d State: %d Lock: %d\n",myproc()->pid,myproc()->state,ptable.lock);
        // cprintf("sleeping %d whose state is %d\n",p1->pid,p1->state);
        // sleep(p1, &ptable.lock);
        // p1->state=SLEEPING;
        p1->sleepschduled=1;
      }
      release(&ptable.lock);
      myproc()->containerjustcalled=0;
      return 0;

    }
  }
}
int sys_leave_container(void){
  if(myproc()->isassignedcontainer!=1){
    //not a container process
    return -1;
  }
  acquire(&ptable.lock);
  struct proc *p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->iscontainer == 1){
      if(p->containerindex==myproc()->containerassigned){
        p->numprocess = p->numprocess-1;
        break;
      }
    }
  }
  release(&ptable.lock);

  myproc()->sleepschduled=0;
  myproc()->isassignedcontainer=0;
  myproc()->containerassigned=-1;
  return 0;
}
int sys_destroy_container(void){
  int a;
  argint(0,&a);
  acquire(&ptable.lock);
  struct proc *p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->iscontainer == 1){
      if(p->containerindex==a){
        // p->numprocess = p->numprocess-1;
        break;
      }
    }
  }
  if(p==&ptable.proc[NPROC]){
    //no such container
    release(&ptable.lock);
    cprintf("no such container\n");
    return -1;
  }
  p->iscontainer=0;
  p->numprocess=0;
  p->containerindex=-1;
  //register an interrupt to exit the 
  release(&ptable.lock);
  kill(p->pid);
  return 0;
}
int togglelog=0;
int sys_registerState(void){
  if(myproc()->iscontainer==0){
    cprintf("not a container\n");
    return -1;
  }
//   int *numprocess;
// // int **processstates;
// // int **sleepschedules;
//   argptr(0, (char **) &(numprocess[myproc()->containerindex -1]),4) ;
//   *(numprocess[myproc()->containerindex -1])=myproc()->numprocess;
//   cprintf("numprocess registered is %d\n",*( numprocess[myproc()->containerindex -1]));
  // cprintf("registered numprocess %d\n",)
  // int relornot;
  // argint(4,&relornot);
  // if(relornot==0){
  int readorwrite;
  argint(0,&readorwrite);
  int tempcontainerindex= myproc()->containerindex;
  if(readorwrite==1){
    // }
    // cprintf("\n starting -:");
    int *numprocess;
    argptr(1, (char **) &numprocess,4);
    *numprocess=myproc()->numprocess;
    int * processstates;
    argptr(2, (char **) &processstates, 4*(myproc()->numprocess));
    int * sleepstates;
    argptr(3, (char **) &sleepstates, 4*(myproc()->numprocess));
    int * containerjustcalledtemp;
    argptr(4, (char **)&containerjustcalledtemp, 4);
    int * syscallping;
    argptr(5, (char **) &syscallping, 4*(myproc()->numprocess));
    int * typesyscall;
    argptr(6, (char **) &typesyscall, 4);
    int i=0;
    struct proc *p;
    int breaksyscall=0;
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->isassignedcontainer == 1){
        if(p->containerassigned==tempcontainerindex){
          // p->numprocess = p->numprocess-1;
          processstates[i]=(int) p->state;
          sleepstates[i]=p->sleepschduled;
          // p->sleepschduled=1;
          syscallping[i]=0;
          if(p->hasdonesyscall==1 && breaksyscall==0){
            syscallping[i]=1;
            int typesyscalltemp = p->typesyscall;
            *typesyscall = typesyscalltemp;
            if(typesyscalltemp==CREATE){
              char * bufchartemp;
              argptr(7,(char **) &bufchartemp,sizeof(char)*30);
              for(int i=0;i<30;i++){
                bufchartemp[i]=p->bufchar[i];
              }

            }else if(typesyscalltemp==OPEN){
              char * bufchartemp;
              argptr(7,(char **) &bufchartemp,sizeof(char)*30);
              for(int i=0;i<30;i++){
                bufchartemp[i]=p->bufchar[i];
              }
            }else if(typesyscalltemp==WRITE){
              char * bufchartemp;
              argptr(7,(char **) &bufchartemp,sizeof(char)*30);
              cprintf("kernel write encountered: %s\n", bufchartemp);
              for(int i=0;i<30;i++){
                bufchartemp[i]=p->bufchar[i];
              }
              int *whichfile;
              argptr(8,(char **) &whichfile,4);
              *whichfile = p->fd;
            }else if(typesyscalltemp==CAT){
              char * bufchartemp;
              argptr(7,(char **) &bufchartemp,sizeof(char)*30);
              for(int i=0;i<30;i++){
                bufchartemp[i]=p->bufchar[i];
              }
            }else if(typesyscalltemp==MALLOC){
              int * bufchartemp;
              argptr(10,(char **) &bufchartemp,4);
              *bufchartemp=p->mallocbuf;
            }else if(typesyscalltemp==MALLOC_WRITE){
              int * bufchartemp;
              argptr(9,(char **) &bufchartemp,4);
              *bufchartemp=p->mallocaddr;
              argptr(10,(char **) &bufchartemp,4);
              *bufchartemp=p->mallocbuf;
            }else if(typesyscalltemp==MALLOC_READ){
              int * bufchartemp;
              argptr(9,(char **) &bufchartemp,4);
              *bufchartemp=p->mallocaddr;
            }
            breaksyscall=1;
          }else{
            syscallping[i]=0;
          }
          i++;
        }
      }
    }
    int *toggletemp;
    argptr(11,(char **) &toggletemp,4);
    *toggletemp = togglelog;
    release(&ptable.lock);

    *containerjustcalledtemp=myproc()->containerjustcalled;
    // cprintf("-returning\n");
    // if(relornot==1){
  }else if(readorwrite==2){
    int *numprocesstemp;
    int *processstatetemp;
    int *sleepscheduletemp;
    int *containerjustcalledtemp;
    int *syscallping;
    argptr(1,(char **) &numprocesstemp,4);
    argptr(2,(char **)&processstatetemp,4* (*numprocesstemp));
    argptr(3,(char **)&sleepscheduletemp,4* (*numprocesstemp));
    argptr(4,(char **)&containerjustcalledtemp,4);
    argptr(5,(char **)&syscallping,4* (*numprocesstemp));
    int piter=0;
    acquire(&ptable.lock);
    struct proc *p;
    // cprintf("writing\n");
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->isassignedcontainer == 1){
        if(p->containerassigned==tempcontainerindex){
          p->sleepschduled=sleepscheduletemp[piter];
          if(p->hasdonesyscall==1){
            if(syscallping[piter]==2){
              int typesyscall=p->typesyscall;
              if(typesyscall==CREATE){
                int* fdargme;
                argptr(8,(char **)&fdargme,4);

                p->fd=*fdargme;
                cprintf("Process fd: %d\n", p->fd);

              }else if(typesyscall==OPEN){
                int* fdargme;
                argptr(8,(char**)&fdargme,4);

                p->fd=*fdargme;
              }else if(typesyscall==WRITE){
                //nothing
              }else if(typesyscall==CAT){
                //nothing
              }else if(typesyscall==MALLOC){
                int* mallocadd;
                argptr(9,(char **)&mallocadd,4);

                p->mallocaddr=*mallocadd;
              }else if(typesyscall==MALLOC_READ){
                int* mallocbuf;
                argptr(10,(char **)&mallocbuf,4);

                p->mallocbuf=*mallocbuf;
              }
              p->hasdonesyscall=0;
              p->typesyscall=-1;
              p->isSysCallComplete=1;
              
            }
          }
          // cprintf("pid %d sleep %d\n",p->pid,p->sleepschduled);
          piter++;
          if(piter>=*numprocesstemp){
            break;
          }
        }
      }
    }
    // cprintf("\n");
    myproc()->containerjustcalled=*containerjustcalledtemp;
    release(&ptable.lock);
  }


  // }
    // cprintf("#");
  return 0;
}

int sys_registerSysCall(void){
  myproc()->hasdonesyscall=1;
  int a;
  argint(0,&a);

  myproc()->typesyscall=a;
  myproc()->isSysCallComplete=0;
  return 0;
}
int sys_getStatusSysCall(void){
  if(myproc()->isSysCallComplete==1){
    return 0;
  }else{
    return -1;
  }
}
int sys_getfd(void){
  int *temp;
  argptr(0,(char **)&temp,4);
  *temp=myproc()->fd;
  cprintf("Proc fd in getfd: %d\n", myproc()->fd);
  return 0;
}
int sys_cat_sys(void){
  cprintf("cat called\n");
  myproc()->hasdonesyscall=1;
  char * tempbuf;
  argptr(0, &tempbuf,30*sizeof(char));
  myproc()->typesyscall=CAT;
  myproc()->isSysCallComplete=0;
  for(int i=0;i<30;i++){
    myproc()->bufchar[i]=tempbuf[i];
  }
  cprintf("Temp buf from cat: %s \n", tempbuf);

  return 0;
}
int sys_ls_sys(void){
  myproc()->hasdonesyscall=1;
  myproc()->typesyscall=LS;
  myproc()->isSysCallComplete=0;
  return 0;
}
int sys_malloc_sys(void){
  myproc()->hasdonesyscall=1;
  myproc()->typesyscall=MALLOC;
  myproc()->isSysCallComplete=0;
  int *temp;
  argptr(0,(char **)&temp,4);
  myproc()->mallocbuf=*temp;
  return 0;
}
int sys_getmallocaddr_sys(void){
  int *temp;
  argptr(0,(char **)&temp,4);
  *temp=myproc()->mallocaddr;
  return 0;
}
int sys_writemalloc_sys(void){
  myproc()->hasdonesyscall=1;
  myproc()->typesyscall=MALLOC_WRITE;
  myproc()->isSysCallComplete=0;
  int *temp;
  argptr(0,(char **)&temp,4);
  myproc()->mallocaddr=*temp;
  argptr(1,(char **)&temp,4);
  myproc()->mallocbuf=*temp;
  return 0;
}
int sys_readmalloc_sys(void){
  myproc()->hasdonesyscall=1;
  myproc()->typesyscall=MALLOC_READ;
  myproc()->isSysCallComplete=0;
  int *temp;
  argptr(0,(char **)&temp,4);
  myproc()->mallocaddr=*temp;
  return 0;
}
int sys_readvalmalloc_sys(void){
  int *temp;
  argptr(0,(char **)&temp,4);
  *temp=myproc()->mallocbuf;
  return 0;  
}
int sys_toggle_log(void){
  int *temp;
  argptr(0,(char **)&temp,4);
  togglelog=*temp;
  return 0;
}