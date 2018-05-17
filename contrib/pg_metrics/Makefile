# contrib/pg_metrics/Makefile

MODULE_big = pg_metrics
OBJS = pg_metrics.o $(WIN32RES)

EXTENSION = pg_metrics
DATA = 
PGFILEDESC = "pg_metrics - prometheus metrics exporter"

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/pg_metrics
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
