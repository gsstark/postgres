/* contrib/memory_tests/memory_tests--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION memory_tests" to load this file. \quit

CREATE FUNCTION pg_memory_test_read_before() RETURNS int AS 'MODULE_PATHNAME' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_read_after() RETURNS int AS 'MODULE_PATHNAME' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_read_undefined() RETURNS int AS 'MODULE_PATHNAME' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_write_before() RETURNS int AS 'MODULE_PATHNAME' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_write_after() RETURNS int AS 'MODULE_PATHNAME' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_doublefree() RETURNS int AS 'MODULE_PATHNAME' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_read_after_free() RETURNS int AS 'MODULE_PATHNAME' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_write_after_free() RETURNS int AS 'MODULE_PATHNAME' LANGUAGE C STRICT VOLATILE;

REVOKE EXECUTE ON pg_memory_test_read_before() FROM PUBLIC;
REVOKE EXECUTE ON pg_memory_test_read_after() FROM PUBLIC;
REVOKE EXECUTE ON pg_memory_test_read_undefined() FROM PUBLIC;
REVOKE EXECUTE ON pg_memory_test_write_before() FROM PUBLIC;
REVOKE EXECUTE ON pg_memory_test_write_after() FROM PUBLIC;
REVOKE EXECUTE ON pg_memory_test_doublefree() FROM PUBLIC;
REVOKE EXECUTE ON pg_memory_test_read_after_free() FROM PUBLIC;
REVOKE EXECUTE ON pg_memory_test_write_after_free() FROM PUBLIC;
