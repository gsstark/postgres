CREATE FUNCTION pg_memory_test_read_before() RETURNS int AS '/home/stark/src/postgresql/contrib/memory_tests/memory_tests.so' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_read_after() RETURNS int AS '/home/stark/src/postgresql/contrib/memory_tests/memory_tests.so' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_read_undefined() RETURNS int AS '/home/stark/src/postgresql/contrib/memory_tests/memory_tests.so' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_write_before() RETURNS int AS '/home/stark/src/postgresql/contrib/memory_tests/memory_tests.so' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_write_after() RETURNS int AS '/home/stark/src/postgresql/contrib/memory_tests/memory_tests.so' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_doublefree() RETURNS int AS '/home/stark/src/postgresql/contrib/memory_tests/memory_tests.so' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_read_after_free() RETURNS int AS '/home/stark/src/postgresql/contrib/memory_tests/memory_tests.so' LANGUAGE C STRICT VOLATILE;
CREATE FUNCTION pg_memory_test_write_after_free() RETURNS int AS '/home/stark/src/postgresql/contrib/memory_tests/memory_tests.so' LANGUAGE C STRICT VOLATILE;

REVOKE EXECUTE ON FUNCTION pg_memory_test_read_before() FROM PUBLIC;
REVOKE EXECUTE ON FUNCTION pg_memory_test_read_after() FROM PUBLIC;
REVOKE EXECUTE ON FUNCTION pg_memory_test_read_undefined() FROM PUBLIC;
REVOKE EXECUTE ON FUNCTION pg_memory_test_write_before() FROM PUBLIC;
REVOKE EXECUTE ON FUNCTION pg_memory_test_write_after() FROM PUBLIC;
REVOKE EXECUTE ON FUNCTION pg_memory_test_doublefree() FROM PUBLIC;
REVOKE EXECUTE ON FUNCTION pg_memory_test_read_after_free() FROM PUBLIC;
REVOKE EXECUTE ON FUNCTION pg_memory_test_write_after_free() FROM PUBLIC;
