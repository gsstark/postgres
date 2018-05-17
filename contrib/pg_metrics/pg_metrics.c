/*-------------------------------------------------------------------------
 *
 * pg_metrics.c
 *
 * Implement a server to serve up metrics from pg_stats 
 *
 * This handles pull-style monitoring systems like Prometheus, and provides as
 * simple HTTP server to provide the stats.
 *
 *	Copyright (c) 2018, PostgreSQL Global Development Group
 *
 *	IDENTIFICATION
 *		contrib/pg_metrics/pg_metrics.c
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"
#include <unistd.h>

#include "access/heapam.h"
#include "access/xact.h"
#include "catalog/pg_class.h"
#include "catalog/pg_type.h"
#include "miscadmin.h"
#include "pgstat.h"
#include "postmaster/bgworker.h"
#include "storage/buf_internals.h"
#include "storage/dsm.h"
#include "storage/ipc.h"
#include "storage/latch.h"
#include "storage/lwlock.h"
#include "storage/proc.h"
#include "storage/procsignal.h"
#include "storage/shmem.h"
#include "storage/smgr.h"
#include "tcop/tcopprot.h"
#include "utils/acl.h"
#include "utils/guc.h"
#include "utils/memutils.h"
#include "utils/rel.h"
#include "utils/relfilenodemap.h"
#include "utils/resowner.h"

PG_MODULE_MAGIC;

#define HTTP_URL "/metrics"
#define DEFAULT_PG_METRICS_PORT 9432
#define DEFAULT_PG_METRICS_ADDR "*"

void		_PG_init(void);
void		pg_metrics_main(Datum main_arg);

static void pgm_start_worker(void);
static void pgm_sigterm_handler(SIGNAL_ARGS);
static void pgm_sighup_handler(SIGNAL_ARGS);

/* Flags set by signal handlers */
static volatile sig_atomic_t got_sigterm = false;
static volatile sig_atomic_t got_sighup = false;

/* GUC variables. */
static bool pg_metrics = true; /* start worker? */
static int	pg_metrics_port;
static char *pg_metrics_addr;

/*
 * Module load callback.
 */
void
_PG_init(void)
{
	DefineCustomIntVariable("pg_metrics.port",
							"The TCP Port to listen for metric requests",
							NULL, 						/* Long desc */
							&pg_metrics_port, 			/* valueAddr */
							DEFAULT_PG_METRICS_PORT,	/* bootValue */
							1, 65535,					/* min, max */
							PGC_SIGHUP,					/* context */
							0,							/* flags */
							NULL,						/* check_hook */
							NULL,						/* assign_hook */
							NULL);						/* show_hook */
	DefineCustomStringVariable("pg_metrics.listen_adresses",
							"The IP addresses to listen for metric requests",
							NULL, 						/* Long desc */
							&pg_metrics_addr, 			/* valueAddr */
							DEFAULT_PG_METRICS_ADDR,	/* bootValue */
							PGC_SIGHUP,					/* context */
							0,							/* flags */
							NULL,						/* check_hook */
							NULL,						/* assign_hook */
							NULL);						/* show_hook */

	if (!process_shared_preload_libraries_in_progress)
		return;

	/* can't define PGC_POSTMASTER variable after startup */
	DefineCustomBoolVariable("pg_metrics.enable",
							 "Starts the pg_metrics web server worker.",
							 NULL,
							 &pg_metrics,
							 true,
							 PGC_POSTMASTER,
							 0,
							 NULL,
							 NULL,
							 NULL);

	EmitWarningsOnPlaceholders("pg_metrics");

	/* Register pg_metrics worker, if enabled. */
	if (pg_metrics)
		pgm_start_worker();
}


/*
 * Start pg_metrics master worker process.
 */
static void
pgm_start_worker(void)
{
	BackgroundWorker worker;
	BackgroundWorkerHandle *handle;
	BgwHandleStatus status;
	pid_t		pid;

	MemSet(&worker, 0, sizeof(BackgroundWorker));
	worker.bgw_flags = BGWORKER_SHMEM_ACCESS;
	worker.bgw_start_time = BgWorkerStart_ConsistentState;
	strcpy(worker.bgw_library_name, "pg_metrics");
	strcpy(worker.bgw_function_name, "pg_metrics_main");
	strcpy(worker.bgw_name, "pg_metrics");
	strcpy(worker.bgw_type, "pg_metrics");

	if (process_shared_preload_libraries_in_progress)
	{
		RegisterBackgroundWorker(&worker);
		return;
	}

	/* must set notify PID to wait for startup */
	worker.bgw_notify_pid = MyProcPid;

	if (!RegisterDynamicBackgroundWorker(&worker, &handle))
		ereport(ERROR,
				(errcode(ERRCODE_INSUFFICIENT_RESOURCES),
				 errmsg("could not register background process"),
				 errhint("You may need to increase max_worker_processes.")));

	status = WaitForBackgroundWorkerStartup(handle, &pid);
	if (status != BGWH_STARTED)
		ereport(ERROR,
				(errcode(ERRCODE_INSUFFICIENT_RESOURCES),
				 errmsg("could not start background process"),
				 errhint("More details may be available in the server log.")));
}


/*
 * Main entry point for the pg_metrics process.
 */
void
pg_metrics_main(Datum main_arg)
{
	/* Establish signal handlers; once that's done, unblock signals. */
	pqsignal(SIGTERM, pgm_sigterm_handler);
	pqsignal(SIGHUP, pgm_sighup_handler);
	pqsignal(SIGUSR1, procsignal_sigusr1_handler);
	BackgroundWorkerUnblockSignals();

	elog(LOG, "pg_metrics starting");

	/* Periodically dump buffers until terminated. */
	while (!got_sigterm)
	{
		int			rc;

		/* In case of a SIGHUP, just reload the configuration. */
		if (got_sighup)
		{
			got_sighup = false;
			ProcessConfigFile(PGC_SIGHUP);
		}

		elog(LOG, "pg_metrics looping");

		/* Sleep for 1s. */
		rc = WaitLatch(&MyProc->procLatch,
					   WL_LATCH_SET | WL_TIMEOUT | WL_POSTMASTER_DEATH,
					   1000,
					   PG_WAIT_EXTENSION);
		
		/* Reset the latch, bail out if postmaster died, otherwise loop. */
		ResetLatch(&MyProc->procLatch);
		if (rc & WL_POSTMASTER_DEATH)
			proc_exit(1);
	}
	elog(LOG, "pg_metrics exiting");
}


/*
 * Signal handler for SIGTERM
 */
static void
pgm_sigterm_handler(SIGNAL_ARGS)
{
	int			save_errno = errno;

	got_sigterm = true;

	if (MyProc)
		SetLatch(&MyProc->procLatch);

	errno = save_errno;
}

/*
 * Signal handler for SIGHUP
 */
static void
pgm_sighup_handler(SIGNAL_ARGS)
{
	int			save_errno = errno;

	got_sighup = true;

	if (MyProc)
		SetLatch(&MyProc->procLatch);

	errno = save_errno;
}
