/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log:	rtc.h,v $
 * Revision 1.5.1.3  90/11/27  13:45:25  rvb
 * 	Synched 2.5 & 3.0 at I386q (r1.5.1.3) & XMK35 (r2.4)
 * 	[90/11/15            rvb]
 * 
 * Revision 1.5.1.2  90/07/27  11:27:06  rvb
 * 	Fix Intel Copyright as per B. Davies authorization.
 * 	[90/07/27            rvb]
 * 
 * Revision 2.2  90/05/03  15:46:11  dbg
 * 	First checkin.
 * 
 * Revision 1.5.1.1  90/01/08  13:29:46  rvb
 * 	Add Intel copyright.
 * 	[90/01/08            rvb]
 * 
 * Revision 1.5  89/09/25  12:27:37  rvb
 * 	File was provided by Intel 9/18/89.
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

#define RTC_ADDR	0x70	/* I/O port address for register select */
#define RTC_DATA	0x71	/* I/O port address for data read/write */

/*
 * Register A definitions
 */
#define RTC_A		0x0a	/* register A address */
#define RTC_UIP		0x80	/* Update in progress bit */
#define RTC_DIV0	0x00	/* Time base of 4.194304 MHz */
#define RTC_DIV1	0x10	/* Time base of 1.048576 MHz */
#define RTC_DIV2	0x20	/* Time base of 32.768 KHz */
#define RTC_RATE6	0x06	/* interrupt rate of 976.562 */

/*
 * Register B definitions
 */
#define RTC_B		0x0b	/* register B address */
#define RTC_SET		0x80	/* stop updates for time set */
#define RTC_PIE		0x40	/* Periodic interrupt enable */
#define RTC_AIE		0x20	/* Alarm interrupt enable */
#define RTC_UIE		0x10	/* Update ended interrupt enable */
#define RTC_SQWE	0x08	/* Square wave enable */
#define RTC_DM		0x04	/* Date mode, 1 = binary, 0 = BCD */
#define RTC_HM		0x02	/* hour mode, 1 = 24 hour, 0 = 12 hour */
#define RTC_DSE		0x01	/* Daylight savings enable */

/* 
 * Register C definitions
 */
#define RTC_C		0x0c	/* register C address */
#define RTC_IRQF	0x80	/* IRQ flag */
#define RTC_PF		0x40	/* PF flag bit */
#define RTC_AF		0x20	/* AF flag bit */
#define RTC_UF		0x10	/* UF flag bit */

/*
 * Register D definitions
 */
#define RTC_D		0x0d	/* register D address */
#define RTC_VRT		0x80	/* Valid RAM and time bit */

#define RTC_NREG	0x0e	/* number of RTC registers */
#define RTC_NREGP	0x0a	/* number of RTC registers to set time */

#define RTCRTIME	_IOR(c, 0x01, struct rtc_st) /* Read time from RTC */
#define RTCSTIME	_IOW(c, 0x02, struct rtc_st) /* Set time into RTC */

struct rtc_st {
	char	rtc_sec;
	char	rtc_asec;
	char	rtc_min;
	char	rtc_amin;
	char	rtc_hr;
	char	rtc_ahr;
	char	rtc_dow;
	char	rtc_dom;
	char	rtc_mon;
	char	rtc_yr;
	char	rtc_statusa;
	char	rtc_statusb;
	char	rtc_statusc;
	char	rtc_statusd;
};

/*
 * this macro reads contents of real time clock to specified buffer 
 */
#define load_rtc(regs) \
{\
	register int i; \
	\
	for (i = 0; i < RTC_NREG; i++) { \
		outb(RTC_ADDR, i); \
		regs[i] = inb(RTC_DATA); \
	} \
}

/*
 * this macro writes contents of specified buffer to real time clock 
 */ 
#define save_rtc(regs) \
{ \
	register int i; \
	for (i = 0; i < RTC_NREGP; i++) { \
		outb(RTC_ADDR, i); \
		outb(RTC_DATA, regs[i]);\
	} \
}	


