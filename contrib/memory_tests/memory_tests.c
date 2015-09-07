/*-------------------------------------------------------------------------
 *
 * memory_tests.so
 *
 *		  A module to *intentionally* crash the database by violating the
 *		  memory management contracts. This is intended to be used under
 *		  Valgrind or other memory checking tools to test whether the Postgres
 *		  hooks in palloc/pfree are correctly marking the memory so they can
 *		  detect errors.
 *
 *        THIS MODULE IS NOT USEFUL IN A PRODUCTION DATABASE. 
 *
 *        IT IS A DEVELOOPER TOOL ONLY
 *
 *
 * Copyright (c) 2015, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *		  contrib/memory_tests/memory_tests.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "miscadmin.h"

#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/memutils.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(pg_memory_test_read_before);
PG_FUNCTION_INFO_V1(pg_memory_test_read_after);
PG_FUNCTION_INFO_V1(pg_memory_test_read_undefined);
PG_FUNCTION_INFO_V1(pg_memory_test_write_before);
PG_FUNCTION_INFO_V1(pg_memory_test_write_after);
PG_FUNCTION_INFO_V1(pg_memory_test_doublefree);
PG_FUNCTION_INFO_V1(pg_memory_test_read_after_free);
PG_FUNCTION_INFO_V1(pg_memory_test_write_after_free);

static void assert_superuser() {
	if (!superuser())
		elog(ERROR, "pg_memory_test_* require super-user");
}


Datum pg_memory_test_read_before(PG_FUNCTION_ARGS)
{
	char *p, t;
	assert_superuser();
	p = palloc(1);
	t = p[-1];
	if ((t) == 0x70) {
		PG_RETURN_INT32(0);
	} else {
		PG_RETURN_INT32(1);
	}
}
Datum pg_memory_test_read_after(PG_FUNCTION_ARGS)
{
	char *p, t;
	assert_superuser();
	p = palloc(1);
	t = p[1];
	if ((t) == 0x70) {
		PG_RETURN_INT32(0);
	} else {
		PG_RETURN_INT32(1);
	}
}
Datum pg_memory_test_read_undefined(PG_FUNCTION_ARGS)
{
	char *p, t;
	assert_superuser();
	p = palloc(1);
	t = p[0];
	if ((t) == 0x70) {
		PG_RETURN_INT32(0);
	} else {
		PG_RETURN_INT32(1);
	}
}
Datum pg_memory_test_write_before(PG_FUNCTION_ARGS)
{
	char *p;
	assert_superuser();
	p = palloc(1);
	p[-1] = 0x70;
	PG_RETURN_INT32(0);
}
Datum pg_memory_test_write_after(PG_FUNCTION_ARGS)
{
	char *p;
	assert_superuser();
	p = palloc(1);
	p[-1] = 0x70;
	PG_RETURN_INT32(0);
}
Datum pg_memory_test_doublefree(PG_FUNCTION_ARGS)
{
	char *p;
	assert_superuser();
	p = palloc(1);
	pfree(p);
	pfree(p);
	PG_RETURN_INT32(0);
}
Datum pg_memory_test_read_after_free(PG_FUNCTION_ARGS)
{
	char *p, t;
	assert_superuser();
	p = palloc(1);
	p[0] = 0x70;
	pfree(p);
	t = p[0];
	PG_RETURN_INT32(t);
}
Datum pg_memory_test_write_after_free(PG_FUNCTION_ARGS)
{
	char *p;
	assert_superuser();
	p = palloc(1);
	p[0] = 0x70;
	pfree(p);
	p[0] = 0x71;
	PG_RETURN_INT32(0);
}
