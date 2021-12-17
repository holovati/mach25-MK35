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
 * $Log:	tcp_fsm.h,v $
 * Revision 2.3  89/03/09  20:46:17  rpd
 * 	More cleanup.
 * 
 * Revision 2.2  89/02/25  19:01:21  gm0w
 * 	Updated copyright.
 * 	[89/02/12            gm0w]
 * 
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)tcp_fsm.h	7.1 (Berkeley) 6/5/86
 */

/*
 * TCP FSM state definitions.
 * Per RFC793, September, 1981.
 */

#define TCP_NSTATES	11

#define TCPS_CLOSED		0	/* closed */
#define TCPS_LISTEN		1	/* listening for connection */
#define TCPS_SYN_SENT		2	/* active, have sent syn */
#define TCPS_SYN_RECEIVED	3	/* have send and received syn */
/* states < TCPS_ESTABLISHED are those where connections not established */
#define TCPS_ESTABLISHED	4	/* established */
#define TCPS_CLOSE_WAIT		5	/* rcvd fin, waiting for close */
/* states > TCPS_CLOSE_WAIT are those where user has closed */
#define TCPS_FIN_WAIT_1		6	/* have closed, sent fin */
#define TCPS_CLOSING		7	/* closed xchd FIN; await FIN ACK */
#define TCPS_LAST_ACK		8	/* had fin and close; await FIN ACK */
/* states > TCPS_CLOSE_WAIT && < TCPS_FIN_WAIT_2 await ACK of FIN */
#define TCPS_FIN_WAIT_2		9	/* have closed, fin is acked */
#define TCPS_TIME_WAIT		10	/* in 2*msl quiet wait after close */

#define TCPS_HAVERCVDSYN(s)	((s) >= TCPS_SYN_RECEIVED)
#define TCPS_HAVERCVDFIN(s)	((s) >= TCPS_TIME_WAIT)

#ifdef	TCPOUTFLAGS
/*
 * Flags used when sending segments in tcp_output.
 * Basic flags (TH_RST,TH_ACK,TH_SYN,TH_FIN) are totally
 * determined by state, with the proviso that TH_FIN is sent only
 * if all data queued for output is included in the segment.
 */
u_char	tcp_outflags[TCP_NSTATES] = {
    TH_RST|TH_ACK, 0, TH_SYN, TH_SYN|TH_ACK,
    TH_ACK, TH_ACK,
    TH_FIN|TH_ACK, TH_FIN|TH_ACK, TH_FIN|TH_ACK, TH_ACK, TH_ACK,
};
#endif

#ifdef	KPROF
int	tcp_acounts[TCP_NSTATES][PRU_NREQ];
#endif

#ifdef	TCPSTATES
char *tcpstates[] = {
	"CLOSED",	"LISTEN",	"SYN_SENT",	"SYN_RCVD",
	"ESTABLISHED",	"CLOSE_WAIT",	"FIN_WAIT_1",	"CLOSING",
	"LAST_ACK",	"FIN_WAIT_2",	"TIME_WAIT",
};
#endif
