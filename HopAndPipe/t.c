
#include "type.h"
int color;

PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;

int body();
char *pname[]={"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter",
               "Saturn", "Uranus", "Neptune" };

/**************************************************
  bio.o, queue.o loader.o are in mtxlib
**************************************************/

#include "kernel.c"
#include "int.c"
#include "wait.c"


int init()
{
    PROC *p;
    int i;
    //color= 0x0A;
    printf("init ....");

    for (i=0; i<NPROC; i++){   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;
        strcpy(proc[i].name, pname[i]);

        p->next = &proc[i+1];
    }
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyQueue = sleepList = 0;

    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = RUNNING;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    nproc = 1;
    printf("done\n");
}

int printSleepList(char *name, PROC *p)
{
   int i = 0;
   printf("-------------------------- %s --------------------------------\n", name);
   for(i = 0; i < NPROC; i++)
   {
      if(proc[i].status == SLEEP)
        printf("%d[e=%d] -> ", proc[i].pid, proc[i].event);
   }
   printf("NULL\n");
}

int scheduler()
{
    if (running->status == RUNNING)
    {
       running->status = READY;
    }
    enqueue(&readyQueue, running);
    running = dequeue(&readyQueue);
    running->status = RUNNING;
}

int int80h();

int set_vec(u16 vector, u16 addr)
{
    u16 location,cs;
    location = vector << 2;
    put_word(addr, 0, location);
    put_word(0x1000,0,location+2);
}

int main()
{
    printf("MTX starts in main()\n");
    init();      // initialize and create P0 as running
    set_vec(80,int80h);

    kfork("/bin/u1");     // P0 kfork() P1
    while(1){
      printf("P0 running\n");
      if (nproc==2 && proc[1].status != READY)
	  printf("no runable process, system halts\n");
      while(!readyQueue);
      printf("P0 switch process\n");
      tswitch();   // P0 switch to run P1
   }
}
