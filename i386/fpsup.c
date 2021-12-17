/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log:	fpsup.c,v $
 * Revision 2.3.1.9  90/07/27  11:23:41  rvb
 * 	Fix Intel Copyright as per B. Davies authorization.
 * 	[90/07/27            rvb]
 * 
 * Revision 2.3.1.8  90/06/07  08:04:50  rvb
 * 	Don't zero out the exception status word in fprestore. [kupfer]
 * 
 * Revision 2.3.1.7  90/02/28  15:47:42  rvb
 * 	Revision 1.15  90/02/26  17:53:00  kupfer
 * 		Synch with CMU.  h/w workarounds ifdef'd by h/w type.
 * 		Lots of code changes.
 * 
 * Revision 2.3.1.6  90/01/16  15:53:55  rvb
 *	Allow finit, fldcw, fstcw w/o fp unit -- just do nothing.
 *	[90/01/10            rvb]
 *   
 * Revision 2.3.1.5  90/01/08  13:29:11  rvb
 *	Add Intel copyright.
 *	[90/01/08            rvb]
 *   
 * Revision 2.3.1.4  90/01/02  13:45:06  rvb
 *	Deleted some old "asm" statements.
 *   
 * Revision 2.3.1.3  89/12/21  17:51:02  rvb
 *	Kill process who does fp if no hardware or no emulator.
 *    
 * Revision 2.3.1.2  89/10/30  12:25:51  rvb
 * 	Revision 1.8  89/10/25  14:58:11  kupfer
 * 	Use fp_thread, not current_thread, in fpexterrflt.
 * 
 * Revision 2.3  89/09/25  12:24:25  rvb
 * 	Make fp configurable and fp.h  -> fpreg.h
 * 	[89/09/23            rvb]
 * 
 * 	             intel corporation proprietary information
 * 	[89/09/23            rvb]
 * 
 */

/*
  Copyright 1988, 1989 by Intel Corporation, Santa Clara, California.

		All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Intel
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/*
 * routines that deal with floating point
 */
#include	<fp.h>

#include <cputypes.h>
#include <sys/types.h>
#include <sys/param.h>
#include <i386/fpreg.h>
#include <i386/pcb.h>
#include <i386/reg.h>
#include <i386/machparam.h>
#include <mach/exception.h>
#include <mach/kern_return.h>
#include <sys/user.h>
#include <sys/proc.h>

char    fp_kind;        /* kind of floating point hardware or emulation */
struct thread *fp_thread;   /* owner of floating point unit */

int     fpsw;           /* status word */
int     finitstate;     /* control word during initialization */

/*
** fpnoextflt
**      handle a coprocessor not present fault
**
**      This fault (INT 7) occurs under two conditions:
**
**      1)  There is a coprocessor, and a floating point instruction
**          is encountered when the task-switched (TS) bit is set.
**          We save and restore floating point states.
**
**      2)  We are doing floating point emulation, and a floating point
**          instruction is encountered.  We emulate the instruction.
*/
fpnoextflt(r0ptr)
	int    *r0ptr;         /* pointer to registers on stack */
{

	_clts();

	/*
	** If the current thread does not own the coprocessor,
	** save the fp state in the owner's pcb structure,
	** and restore or establish the current thread's fp state.
	** fpsave() sets the TS bit, so we have to clear it.
	*/
	if (fp_thread != current_thread()) {
		if (fp_thread) {
			fpsave();
			_clts();
		}
		fp_thread = current_thread();
		/*
		** If the current thread's state is valid, restore it.
		** Otherwise, this is the first time this thread has
		** executed a fp instruction, so initialize the fp
		** unit for it.
		*/
		if (fp_thread->pcb->pcb_fpvalid)
			fprestore();
		else
			fpinit(r0ptr);
	}

	if (fp_kind == FP_SW)		/* software emulation */
#if	NFP > 0
		e80387(r0ptr);		/* call the emulator */
#else	NFP > 0
		fppanic(r0ptr);
#endif	NFP > 0
}

#if	NFP > 0
#else	NFP > 0
fppanic(r0ptr)
int *r0ptr;
{
	struct proc *p = u.u_procp;
	unsigned short *pcp = (unsigned short *)r0ptr[EIP];
	
	if (*pcp == 0xe3db) {		/* finit */
		r0ptr[EIP] += 2;
		return;
	}
	if ( *pcp == 0x7dd9 || *pcp == 0x6dd9) { /* fstcw, fldcw */
		r0ptr[EIP] += 3;
		return;
	}


	/* 
	 *	kill the current process.
	 *	Send SIGKILL, blow away other pending signals.
	 */
	uprintf("You have no hardware or software fp emulator.  Too bad ...");

	p->p_sig = sigmask(SIGKILL);
	p->p_cursig = SIGKILL;
	u.u_sig = 0;
	u.u_cursig = 0;
	psig();		/* Bye */
}
#endif	NFP > 0

/*
** fpextovrflt
**      handle a coprocessor overrun fault (INT 9)
*/
fpextovrflt(r0ptr)
	int    *r0ptr;         /* pointer to registers on stack */
{
	printf("\nEXTOVRFLT: eip = 0x%x\n", r0ptr[EIP]);

	_clts();

	/* 
	 * Re-initialize the coprocessor. "[fninit] may be necessary 
	 * to clear the 80387 if a processor-extension segment-overrun 
	 * exception (interrupt 9) is detected by the CPU."
	 * --80387 Programmer's Reference Manual
  	 */
	fpinit(r0ptr);

	/* 
	 * Send segmentation violation error signal to the thread
	 * that owns the coprocessor.  I assume that fp_thread == 
	 * current_thread here.  If not, we'll find out soon enough.
	 */
	if (fp_thread)
		thread_doexception(fp_thread, EXC_BAD_ACCESS,
				   KERN_INVALID_ADDRESS, 0);
	else {
		printf("\nEXTOVRFLT WARNING: no FP thread\n");
		return;
	}

	/* 
	 * If the current thread is not the thread that owns the coprocessor,
	 * set the TS bit.  (why is this here? -mdk)
	 */
	if (fp_thread != current_thread())
		setts();
}

/*
** fpexterrflt
**      Handle a coprocessor error fault.  This can either be an 
**      exception (INT 16), or on AT's it can be an external interrupt 
**      (see fpintr()).  There doesn't seem to be a good way to force 
**      the AT interrupt to happen before a context switch, so we have 
**      to deal with the possibility that fp_thread != current_thread.
*/
fpexterrflt()
{
	struct i387_state *state;
	thread_t fp_owner = fp_thread;

	/* 
	 * Clear TS bit in CR0.  I'm not sure why this is necessary, 
	 * but maybe it's because interrupts on an AT can happen at 
	 * awkward times.
	 */
	_clts();

	fpsw = 0;			/* clear temporary for status word */

	fpsw = _fnstsw();
	_fnclex();

	/* 
	 * We can't do a thread_doexception to fp_thread, because that 
	 * routine chokes if fp_thread != current_thread.  So, what 
	 * we'll do is store the FP state in the owner's pcb.  If the
	 * current thread is the FP owner, then send it an exception.
	 * Otherwise, put the original status word back into the FP
	 * state so that the FP owner will get a poke from the
	 * coprocessor when it next restores its state.
	 */
	if (!fp_thread)
		panic("unowned coprocessor interrupt.");
	fpsave();			/* (nulls fp_thread) */

	if (fp_owner == current_thread()) {
		fp_owner->pcb->pcb_fps.status = fpsw;
		thread_doexception(current_thread(), EXC_ARITHMETIC,
				   EXC_I386_EXTERR, 0);
	} else {
		/* XXX - does this work for all possible values of fp_kind? */
		state = (struct i387_state *)fp_owner->pcb->pcb_fps.state;
		state->env.status = fpsw;
		fp_owner->pcb->pcb_fps.status = 0;
	}
}

#if	AT386
/*
** fpintr
**      handle a coprocessor error interrupt on the AT386
**
**      this comes in on line 5 of the slave PIC at SPL1
*/
fpintr()
{
	outb(0xF0, 0);		/* Turn off 'busy' to coprocessor */
	fpexterrflt();
}
#endif	AT386

/*
** fpinit
**	Initialize the floating point unit for this user.
**	Note: the longjmp() in libc reinitializes the coprocessor as 
**	well.  If you change the way the coprocessor is set up (e.g., 
**	if you change which exceptions are disabled), be sure to 
**	change the corresponding libc fpinit code.
*/
fpinit(r0ptr)
int *r0ptr;
{
	/* to keep the floating point emulator from looping, we
	 * set fpvalid here when emulating.  remember that the emulator
	 * was keying on fpvalid == 0 to trap to us in the first place.
	 */
	if (fp_kind == FP_SW)
#if	NFP > 0
		current_thread()->pcb->pcb_fpvalid = 1;
#else	NFP > 0
	{
		fppanic(r0ptr);
		return;
	}
#endif	NFP > 0

	_fninit();

	/*
	** must allow invalid operation, zero divide, and
	** overflow interrupt conditions and change to use
	** long real precision
	*/
	finitstate = _fstcw();

	finitstate &= ~( FPINV | FPZDIV | FPOVR | FPPC );
	finitstate |= ( FPSIG53 | FPIC );

	_fldcw(finitstate);

}

/*
** fpsave
**      Save the floating point state into fp_thread's pcb structure, 
**      and mark the coprocessor as having no owner.  The coprocessor 
**      must be marked as "no owner" because savefp() cleared the 
**      coprocessor exceptions after saving the coprocessor state.
**
**      fp_thread must be valid
*/
fpsave( )
{
	void savefp();

	/* if chip present, save its state */
	if (fp_kind & FP_HW)
		savefp(fp_thread->pcb->pcb_fps.state);

	/* Say that the saved state is valid */
	fp_thread->pcb->pcb_fpvalid = 1;

	/* Now nobody owns the fp unit */
	fp_unowned();
}


/* 
 * fp_unowned:
 * 
 * 	Mark the FP unit as not having an owner.  The TS bit is set so 
 * 	that the next time somebody does an FP operation, we'll notice 
 * 	and restore their FP context.
 */
fp_unowned()
{
	fp_thread = THREAD_NULL;
	setts();
}
  
/*
** fprestore
**	restore the floating point state from the current
**	thread's pcb structure
*/
fprestore()
{
	void restorefp();

	/* if chip present, restore its state */
	if (fp_kind & FP_HW)
		restorefp(current_thread()->pcb->pcb_fps.state);

	/* 
	 * Say that the saved state is not valid.  The status word is 
	 * left alone in case someone (e.g., a debugger) wants to see 
	 * what the status was at the previous exception.
	 */
	current_thread()->pcb->pcb_fpvalid = 0;
}
