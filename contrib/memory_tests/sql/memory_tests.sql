CREATE EXTENSION memory_tests;


SELECT pg_memory_test_read_before();
SELECT pg_memory_test_read_after();
SELECT pg_memory_test_read_undefined();
SELECT pg_memory_test_write_before();
SELECT pg_memory_test_write_after();
SELECT pg_memory_test_doublefree();
SELECT pg_memory_test_read_after_free();
SELECT pg_memory_test_write_after_free();

DROP EXTENSION memory_tests;
