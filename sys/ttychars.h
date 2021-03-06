/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log:	ttychars.h,v $
 * Revision 2.3  89/03/09  22:09:08  rpd
 * 	More cleanup.
 * 
 * Revision 2.2  89/02/25  17:57:29  gm0w
 * 	Changes for cleanup.
 * 
 * 18-Feb-86  Bill Bolosky (bolosky) at Carnegie-Mellon University
 *	Added different definition of CERASE for romp.
 *
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ttychars.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_TTYCHARS_H_
#define _SYS_TTYCHARS_H_

/*
 * User visible structures and constants
 * related to terminal handling.
 */

struct ttychars {
	char	tc_erase;	/* erase last character */
	char	tc_kill;	/* erase entire line */
	char	tc_intrc;	/* interrupt */
	char	tc_quitc;	/* quit */
	char	tc_startc;	/* start output */
	char	tc_stopc;	/* stop output */
	char	tc_eofc;	/* end-of-file */
	char	tc_brkc;	/* input delimiter (like nl) */
	char	tc_suspc;	/* stop process signal */
	char	tc_dsuspc;	/* delayed stop process signal */
	char	tc_rprntc;	/* reprint line */
	char	tc_flushc;	/* flush output (toggles) */
	char	tc_werasc;	/* word erase */
	char	tc_lnextc;	/* literal next character */
};

#define CTRL(c)	('c'&037)

/* default special characters */
#ifdef	ibmrt
#define CERASE	CTRL(h)
#else	ibmrt
#define CERASE	0177
#endif	ibmrt
#define CKILL	CTRL(u)
#define CINTR	CTRL(c)
#define CQUIT	034		/* FS, ^\ */
#define CSTART	CTRL(q)
#define CSTOP	CTRL(s)
#define CEOF	CTRL(d)
#define CEOT	CEOF
#define CBRK	0377
#define CSUSP	CTRL(z)
#define CDSUSP	CTRL(y)
#define CRPRNT	CTRL(r)
#define CFLUSH	CTRL(o)
#define CWERASE	CTRL(w)
#define CLNEXT	CTRL(v)

#endif	_SYS_TTYCHARS_H_
