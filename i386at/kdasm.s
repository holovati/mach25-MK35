/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log:	kdasm.s,v $
 * Revision 2.2.1.6  91/05/03  10:58:17  rvb
 * 	Always set "direction bit" before rep.
 * 	[91/05/03            rvb]
 * 
 * Revision 2.2.1.5  91/01/30  08:49:15  rvb
 * 	Follow dbg's lead.
 * 	[91/01/15            rvb]
 * 
 * Revision 2.2.1.4  90/11/27  13:44:27  rvb
 * 	Synched 2.5 & 3.0 at I386q (r2.2.1.4) & XMK35 (r2.3)
 * 	[90/11/15            rvb]
 * 
 * Revision 2.2  90/05/03  15:45:07  dbg
 * 	First checkin.
 * 
 * Revision 2.2.1.3  90/02/28  15:50:42  rvb
 * 	Fix numerous typo's in Olivetti disclaimer.
 * 	[90/02/28            rvb]
 * 
 * Revision 2.2.1.2  90/01/08  13:30:38  rvb
 * 	Add Olivetti copyright.
 * 	[90/01/08            rvb]
 * 
 * Revision 2.2.1.1  89/10/22  11:34:35  rvb
 * 	New a.out and coff compatible .s files.
 * 	[89/10/16            rvb]
 * 
 * Revision 2.2  89/04/05  13:02:14  rvb
 * 	Converted to real asm file.
 * 	[89/03/04            rvb]
 * 
 * Revision 1.3  89/02/26  12:37:20  gm0w
 * 	Changes for cleanup.
 * 
 */
 
/* 
 * Some inline code to speed up major block copies to and from the
 * screen buffer.
 * 
 * Copyright Ing. C. Olivetti & C. S.p.A. 1988, 1989.
 *  All rights reserved.
 *
 * orc!eugene	28 Oct 1988
 *
 */
/*
  Copyright 1988, 1989 by Olivetti Advanced Technology Center, Inc.,
Cupertino, California.

		All Rights Reserved

  Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Olivetti
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

  OLIVETTI DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL OLIVETTI BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUR OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/* $ Header: $ */

 
#include <i386/asm.h>

/*
 * Function:	kd_slmwd()
 *
 *	This function "slams" a word (char/attr) into the screen memory using
 *	a block fill operation on the 386.
 *
 */

#define start 0x08(%ebp)
#define count 0x0c(%ebp)
#define value 0x10(%ebp)

ENTRY(kd_slmwd)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi

	movl	start, %edi
	movl	count, %ecx
	movw	value, %ax
	cld
	rep
	stosw

	popl	%edi
	leave
	ret
#undef start
#undef count
#undef value

/*
 * "slam up"
 */

#define from  0x08(%ebp)
#define to    0x0c(%ebp)
#define count 0x10(%ebp)
ENTRY(kd_slmscu)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%edi

	movl	from, %esi
	movl	to, %edi
	movl	count, %ecx
	cmpl	%edi, %esi
	cld
	rep
	movsw

	popl	%edi
	popl	%esi
	leave
	ret

/*
 * "slam down"
 */
ENTRY(kd_slmscd)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%edi

	movl	from, %esi
	movl	to, %edi
	movl	count, %ecx
	cmpl	%edi, %esi
	std
	rep
	movsw
	cld

	popl	%edi
	popl	%esi
	leave
	ret
#undef from
#undef to
#undef count
