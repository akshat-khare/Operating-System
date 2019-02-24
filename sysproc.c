#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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
  while(temp==-1){
    sleepcustom();
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
