// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "spinlock.h"
#include "proc.h"

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;

  /*
  P5 changes
  */
  uint free_pages; //track free pages
  uint ref_cnt[PHYSTOP / PGSIZE]; //track reference count

} kmem;

extern char end[]; // first address after kernel loaded from ELF file

// Initialize free list of physical pages.
void
kinit(void)
{
  char *p;

  initlock(&kmem.lock, "kmem");
  p = (char*)PGROUNDUP((uint)end);
  kmem.free_pages = 0;
  for(; p + PGSIZE <= (char*)PHYSTOP; p += PGSIZE) {
    kmem.ref_cnt[(uint)p / PGSIZE] = 1;
    kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;
  uint ref;

  if((uint)v % PGSIZE || v < end || (uint)v >= PHYSTOP) 
    panic("kfree");
  else if ((ref = kmem.ref_cnt[(uint)v / PGSIZE]) != 1) {
    // cprintf("%d ref_cnt: %d\n", (uint)v, ref);
    panic("kfree page used by multiple processes.");
    // page must be used no more than 1 process when freed 
  }

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  acquire(&kmem.lock);
  r = (struct run*)v;
  kmem.ref_cnt[(uint)r / PGSIZE] = 0;
  r->next = kmem.freelist;
  kmem.freelist = r;
  kmem.free_pages += 1; // add one free page
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r) {
    // cprintf("Allocated one more page for proc %d\n", proc->pid);
    kmem.freelist = r->next;
    kmem.ref_cnt[(uint)r / PGSIZE] = 1; // when a page is allocated, set ref_cnt to 1
  }
  kmem.free_pages -= 1; // one less free page after kalloc
  release(&kmem.lock);
  return (char*)r;
}

int kgetrefcnt(char *v) {
  return kmem.ref_cnt[(uint)v / PGSIZE];
}

void kincrement(char *v) {
  acquire(&kmem.lock);
  kmem.ref_cnt[(uint)v / PGSIZE]++;
  // cprintf("kincrement to %d by proc id %d\n", (uint)v, proc->pid);
  release(&kmem.lock);
}

void kdecrement(char *v) {
  if (kmem.ref_cnt[(uint)v / PGSIZE] == 1) {
    // cprintf("kfree %d by proc id %d\n", (uint)v, proc->pid);
    kfree(v);
  } else {
    acquire(&kmem.lock);
    // cprintf("kdecrement to %d by proc id %d\n", (uint)v, proc->pid);
    kmem.ref_cnt[(uint)v / PGSIZE]--;
    release(&kmem.lock);
  }
}

int sys_getFreePagesCount(void){
  return kmem.free_pages;
}