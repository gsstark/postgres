/*-------------------------------------------------------------------------
 *
 * rusagestub.h
 *	  Stubs for getrusage(3).
 *
 *
 * Portions Copyright (c) 1996-2010, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/rusagestub.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef RUSAGESTUB_H
#define RUSAGESTUB_H

#include <sys/time.h>			/* for struct timeval */
#ifndef WIN32
#include <sys/times.h>			/* for struct tms */
#endif
#include <limits.h>				/* for CLK_TCK */

#define RUSAGE_SELF		0
#define RUSAGE_CHILDREN (-1)

struct rusage
{
	struct timeval ru_utime;	/* user time used */
	struct timeval ru_stime;	/* system time used */
	long   ru_maxrss;           /* maximum resident set size */
	long   ru_ixrss;            /* integral shared memory size */
	long   ru_idrss;            /* integral unshared data size */
	long   ru_isrss;            /* integral unshared stack size */
	long   ru_minflt;           /* page reclaims */
	long   ru_majflt;           /* page faults */
	long   ru_nswap;            /* swaps */
	long   ru_inblock;          /* block input operations */
	long   ru_oublock;          /* block output operations */
	long   ru_msgsnd;           /* messages sent */
	long   ru_msgrcv;           /* messages received */
	long   ru_nsignals;         /* signals received */
	long   ru_nvcsw;            /* voluntary context switches */
	long   ru_nivcsw;           /* involuntary context switches */
};

extern int	getrusage(int who, struct rusage * rusage);

#endif   /* RUSAGESTUB_H */
