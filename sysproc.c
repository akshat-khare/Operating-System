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