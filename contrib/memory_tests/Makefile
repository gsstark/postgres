# contrib/test_decoding/Makefile

# contrib/pg_trgm/Makefile

MODULE_big = memory_tests
OBJS = memory_tests.o

EXTENSION = memory_tests
DATA = memory_tests--1.0.sql
PGFILEDESC = "memory_tests - functions to intentionallyl fail memory checks"

REGRESS = memory_tests

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/memory_tests
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
