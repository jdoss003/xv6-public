#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(uint id, char **pointer) {
  int i;

  if (id == 0)
  {
    cprintf("Invalid id");
    return -1;
  }

  acquire(&(shm_table.lock));

  for (i = 0; i< 64; i++) {
    if (shm_table.shm_pages[i].id == id)
    {
      if(mappages(myproc()->pgdir, (char*)PGROUNDUP(myproc()->sz), PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U) < 0){
        cprintf("Could not map page to process pgdir\n");
        release(&(shm_table.lock));
        return -1;
      }
      myproc()->sz = PGROUNDUP(myproc()->sz);
      *pointer = (char *)PGROUNDUP(myproc()->sz);
      ++shm_table.shm_pages[i].refcnt;
      release(&(shm_table.lock));
      return 0;
    }
  }

  for (i = 0; i < 64; i++) {
    if (shm_table.shm_pages[i].id == 0)
    {
      break;
    }
  }

  if (i == 64)
  {
    cprintf("No free entry found\n");
    release(&(shm_table.lock));
    return -1;
  }

  shm_table.shm_pages[i].id = id;
  shm_table.shm_pages[i].frame = kalloc();

  memset(shm_table.shm_pages[i].frame, 0, PGSIZE);

  if(mappages(myproc()->pgdir, (char*)PGROUNDUP(myproc()->sz), PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U) < 0){
    cprintf("Could not map page to process pgdir\n");
    shm_table.shm_pages[i].id = 0;
    kfree(shm_table.shm_pages[i].frame);
    release(&(shm_table.lock));
    return -1;
  }

  myproc()->sz = PGROUNDUP(myproc()->sz);
  *pointer = (char *)PGROUNDUP(myproc()->sz);
  shm_table.shm_pages[i].refcnt = 1;


  release(&(shm_table.lock));

  return 0;
}


int shm_close(uint id) {
  int i;
  pte_t *pg;

  if (id == 0)
  {
    cprintf("Invalid id");
    return -1;
  }

  acquire(&(shm_table.lock));

  for (i = 0; i< 64; i++) {
    if (shm_table.shm_pages[i].id == id)
    {
      if (shm_table.shm_pages[i].refcnt < 1)
      {
        panic("Refernce count should not be 0\n");
      }

      pg = walkpgdir(myproc()->pgdir, (const void *)PGROUNDDOWN(myproc()->sz), 0);
      if (pg == 0)
      {
        cprintf("Process tried to close a page it is not sharing\n");
        release(&(shm_table.lock));
        return -1;
      }

      --shm_table.shm_pages[i].refcnt;
      myproc()->sz -= PGSIZE;
      *pg = 0;

      if (shm_table.shm_pages[i].refcnt == 0) {
        shm_table.shm_pages[i].id = 0;
        kfree(shm_table.shm_pages[i].frame);
        shm_table.shm_pages[i].frame = 0;
      }

      release(&(shm_table.lock));
      return 0;
    }
  }

  cprintf("No entry found for id %d\n", id);
  release(&(shm_table.lock));
  return -1; //added to remove compiler warning -- you should decide what to return
}
