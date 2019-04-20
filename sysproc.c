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
int sys_samplecall(void){
  cprintf("been called\n");
  boolwritewalkpage=1;
  return 0;
}
int sys_create_container(void){
  myproc()->iscontainer=1;
  int a;
  argint(0,&a);
  myproc()->containerindex=a;
  myproc()->numprocess=0;
  myproc()->sleepschduled=0;
  return 0;
}
int sys_join_container(void){
  myproc()->isassignedcontainer=1;
  int a;
  argint(0,&a);
  myproc()->containerassigned=a;
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
  // sleepcustom();
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
