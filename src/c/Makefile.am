
TOP_DIR = ../../

INCLUDES = -I../../include -I../include -I.

noinst_LTLIBRARIES = libmg3dc.la

if ENABLE_COLOR16
COLOR_SRC=cscan15.c cscan16.c czscan15.c czscan16.c
endif

if ENABLE_COLOR8
COLOR_SRC=cscan8.c czscan8.c
endif

if ENABLE_COLOR24
COLOR_SRC=cscan24.c czscan24.c
endif

if ENABLE_COLOR32
COLOR_SRC=cscan32.c czscan32.c
endif

COMMON_SRCS =       

libmg3dc_la_SOURCES = $(COMMON_SRCS) $(COLOR_SRC)

EXTRA_DIST=cdefs15.h  cdefs24.h  cdefs8.h   cscan16.c  cscan32.c  cscan.h     czscan16.c  czscan32.c  czscan.h \
           cdefs16.h  cdefs32.h  cscan15.c  cscan24.c  cscan8.c   czscan15.c  czscan24.c  czscan8.c


