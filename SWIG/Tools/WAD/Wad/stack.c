/* ----------------------------------------------------------------------------- 
 * stack.c
 *
 *     This file is used to unwind the C call stack.
 *
 * Author(s) : David Beazley (beazley@cs.uchicago.edu)
 *
 * Copyright (C) 2000.  The University of Chicago
 * See the file LICENSE for information on usage and redistribution.	
 * ----------------------------------------------------------------------------- */

#include "wad.h"
#include <sys/types.h>
#include <sys/mman.h>
#include <dlfcn.h>

#ifdef WAD_SOLARIS
#define STACK_BASE   0xffbf0000
#endif
#ifdef WAD_LINUX
#define STACK_BASE   0xc0000000
#endif

/* Given a stack pointer, this function performs a single level of stack 
   unwinding */

static void
stack_unwind(unsigned long *sp, unsigned long *pc, unsigned long *fp) {

  if (wad_debug_mode & DEBUG_UNWIND) {
    printf("::: stack unwind :  pc = %x, sp = %x, fp = %x\n", *pc, *sp, *fp);
  }
#ifdef WAD_SOLARIS
  *pc = *((unsigned long *) *sp+15);         /* %i7 - Return address  */
  *sp = *((unsigned long *) *sp+14);         /* %i6 - frame pointer   */
#endif

#ifdef WAD_LINUX
  *pc = *((unsigned long *) *fp+1); 
  *sp = *fp;
  *fp = *((unsigned long *) *fp);

  /* If we get a frame pointer of zero, we've gone off the end of the stack.  Set the
     stack pointer to zero to signal the stack unwinder. */

  if (*fp == 0) {
    *sp = 0;
  }
#endif
}

static void *trace_addr = 0;
static int   trace_len  = 0;

/* Perform a stack unwinding given the current value of the pc, stack pointer,
   and frame pointer.  Returns a pointer to a wad exception frame structure
   which is actually a large memory mapped file. */

static char            framefile[256];

WadFrame *
wad_stack_trace(unsigned long pc, unsigned long sp, unsigned long fp) {
  WadSegment      *ws, *segments;
  WadObjectFile       *wo;
  WadFrame        frame;
  WadDebug        wd;
  WadSymbol       wsym;
  int             nlevels;

  int             ffile;
  unsigned long   p_pc;
  unsigned long   p_sp;
  unsigned long   p_fp;
  unsigned long   p_lastsp;

  int             n = 0;
  int             lastsize = 0;
  int             firstframe = 1;

  /* Read the segments */

  if (wad_segment_read() < 0) {
    printf("WAD: Unable to read segment map\n");
    return 0;
  }

  /* Open the frame file for output */
  tmpnam(framefile);
  ffile = open(framefile, O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if (ffile < 0) {
    printf("can't open %s\n", framefile);
    return 0;
  }

  /* Try to do a stack traceback */
  nlevels = 0;
  p_pc = pc;
  p_sp = sp;
  p_fp = fp;

  while (p_sp) {
    /* Add check for stack validity here */
    ws = wad_segment_find((void *) p_sp);

    if (!ws) {
      /* If the stack is bad, we are really hosed here */
      write(1,"Whoa. Stack is corrupted. Bailing out.\n", 39);
      exit(1);
      break;
    }
    ws = wad_segment_find((void *) p_pc);
    {
      int   symsize = 0;
      int   srcsize = 0;
      int   objsize = 0;
      int   stacksize = 0;
      int   argsize = 0;
      char  *srcname = 0;
      char  *objname = 0;
      char  *symname = 0;
      int    pad = 0;
      unsigned long value;
      
      /* Try to load the object file for this address */
      if (ws) {
	wo = wad_object_load(ws->mappath);
	/* Special hack needed for base address */
      }
      else {
	wo = 0;
      }
      
      /* Try to find the symbol corresponding to this PC */
      if (wo) {
	symname = wad_find_symbol(wo, (void *) p_pc, (unsigned long) ws->base, &wsym);
      } else {
	symname = 0;
      }


      /*      if (symname) symname = wad_cplus_demangle(&wsym); */

      value = wsym.value;
      

      /* Build up some information about the exception frame */
      frame.frameno = n;
      frame.last = 0;
      frame.lastsize = lastsize;
      frame.pc = p_pc;
      frame.sp = p_sp;
      frame.nargs = -1;
      frame.arg_off = 0;
      frame.sym_base = value + (long) ws->base;
      n++;
      if (symname) {
	symsize = strlen(symname)+1;
	
	/*	printf("C++: '%s' ---> '%s'\n", symname, wad_cplus_demangle(&wsym));*/
	
	/* Try to gather some debugging information about this symbol */
	if (wad_debug_info(wo,&wsym, p_pc - (unsigned long) ws->base - value, &wd)) {
	  srcname = wd.srcfile;
	  srcsize = strlen(srcname)+1;
	  objname = wd.objfile;
	  objsize = strlen(objname)+1;
	  frame.nargs = wd.nargs;
	  if (wd.nargs > 0) {
	    argsize = sizeof(WadParm)*wd.nargs;
	  }
	  /*
	  if (wd.nargs >=0) {
	    int i;
	    printf("%s\n",symname);
	    for (i = 0; i < wd.nargs; i++) {
	      printf("  [%d] = '%s', %d, %d\n", i, wd.parms[i].name, wd.parms[i].type, wd.parms[i].value);
	    }
	  }
	  */
	  frame.line_number = wd.line_number;
	}
      }


#ifdef WAD_SOLARIS
      /* Before unwinding the stack, copy the locals and %o registers from previous frame */
      if (!firstframe) {
	int i;
	long *lsp = (long *) p_lastsp;
	for (i = 0; i < 16; i++) {
	  /*	  printf("regs[%d] = 0x%x\n", lsp[i]); */
	  frame.regs[i] = lsp[i];
	}
      }
#endif
      firstframe = 0;
      /* Determine stack frame size */
      p_lastsp = p_sp;

      stack_unwind(&p_sp, &p_pc, &p_fp);

      if (p_sp) {
	stacksize = p_sp - p_lastsp;
      } else {
	stacksize = STACK_BASE - p_lastsp;    /* Sick hack alert. Need to get stack top from somewhere */
      }

      /* Sanity check */
      if ((p_sp + stacksize) > STACK_BASE) {
	stacksize = STACK_BASE - p_sp;
      }

      /* Set the frame pointer and stack size */

      /*      frame.fp = p_sp;  */
      frame.fp = p_sp;
      frame.stack_size = stacksize;

      /* Build the exception frame object we'll write */
      frame.size = sizeof(WadFrame) + symsize + srcsize + objsize + stacksize + argsize;
      pad = 8 - (frame.size % 8);  
      frame.size += pad;

      frame.data[0] = 0;
      frame.data[1] = 0;
      
      /* Build up the offsets */
      if (!symname) {
	frame.sym_off = sizeof(WadFrame) - 8;
	frame.src_off = sizeof(WadFrame) - 8;
	frame.obj_off = sizeof(WadFrame) - 8;
	frame.stack_off = sizeof(WadFrame) + argsize;
	frame.arg_off   = 0;
      } else {
	frame.arg_off = sizeof(WadFrame);
	frame.stack_off = sizeof(WadFrame) + argsize;
	frame.sym_off = frame.stack_off + stacksize;
	frame.src_off = frame.sym_off + symsize;
	frame.obj_off = frame.src_off + srcsize;
      }

      write(ffile,&frame,sizeof(WadFrame));
      /* Write the argument data */
      if (argsize > 0) {
	write(ffile, (void *) wd.parms, argsize);
      }
      /* Write the stack data */
      if (stacksize > 0) {
	write(ffile, (void *) p_lastsp, stacksize);
      }
      if (symname) {
	write(ffile,symname,symsize);
	write(ffile,srcname,srcsize);
	write(ffile,objname,objsize);
      }
      write(ffile,frame.data, pad);
      lastsize = frame.size;
      if (wo)
	wad_object_release(wo);
    }
  }

  /* Write terminator */
  frame.size = 0;
  frame.last = 1;
  frame.lastsize = lastsize;
  frame.stack_size = 0;
  write(ffile,&frame,sizeof(WadFrame));
  close(ffile);

  /* mmap the debug file back into memory */

  ffile = open(framefile, O_RDONLY, 0644);  
  trace_len = lseek(ffile,0,SEEK_END);
  lseek(ffile,0,SEEK_SET);
  trace_addr = mmap(NULL, trace_len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, ffile, 0);
  close(ffile);
  return (WadFrame *) trace_addr;
}

void wad_release_trace() {
  char name[256];
  munmap(trace_addr, trace_len);
  unlink(framefile);
  trace_addr = 0;
  trace_len = 0;
}

/* This function steals an argument out of a frame further up the call stack :-) */

long wad_steal_arg(WadFrame *f, char *symbol, int argno, int *error) {
  char *fd;
  long *regs;
  WadFrame *lastf = 0;

  fd = (char *) f;

  *error = 0;
  /* Start searching */
  while (f->size) {
    if (strcmp(SYMBOL(f),symbol) == 0) {
      /* Got a match */
      if (lastf) {
	regs = STACK(f);
	return regs[8+argno];
      }
    }
    lastf = f;
    fd = fd + f->size;
    f = (WadFrame *) fd;
  }
  *error = -1;
  return 0;
}


long wad_steal_outarg(WadFrame *f, char *symbol, int argno, int *error) {
  char *fd;
  long *regs;
  WadFrame *lastf = 0;

  fd = (char *) f;

  *error = 0;
  /* Start searching */
  while (f->size) {
    if (strcmp(SYMBOL(f),symbol) == 0) {
      /* Got a match */
      if (lastf) {
#ifdef WAD_SOLARIS
	regs = STACK(lastf);
	return regs[8+argno];
#endif
#ifdef WAD_LINUX
	regs = STACK(f);
	return regs[argno+2];
#endif
      }
    }
    lastf = f;
    fd = fd + f->size;
    f = (WadFrame *) fd;
  }
  *error = -1;
  return 0;
}







