#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


uint64 sys_hello(void) {
  printf("Hello, world!\n");
  return 0;
}

uint64 sys_trace(void)
{
    int mask;
    argint(0, &mask); // Lấy mask từ user space (0 = tham số đầu của trace(int mask))
    myproc()->trace_mask = mask;
    return 0;
}
extern uint64 count_free_mem(void);
extern uint64 count_procs(void);
extern uint64 count_open_files(void);

uint64 sys_sysinfo(void) {
    struct sysinfo info;
    uint64 addr;

    // Lấy địa chỉ struct sysinfo từ user space
    argaddr(0, &addr);

    info.freemem = count_free_mem();
    info.nproc = count_procs();
    info.nopenfiles = count_open_files();

    // Sao chép struct sysinfo về user space
    if (copyout(myproc()->pagetable, addr, (char*)&info, sizeof(info)) < 0) {
        return -1;
    }
    return 0;
}