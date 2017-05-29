TOP_DIR = ..
INCLUDES = -I../include -Iinclude 

lib_LTLIBRARIES = libmg3d.la

COMMON_SUBDIRS = $(MG3D_SUBDIRS) include 

COMMON_SRCS = colblend.c  color.c  gfx.c  graphics.c poly3d.c clip3d.c clip3df.c 3dmath.c scene3d.c

SUBDIRS = $(COMMON_SUBDIRS) 
DIST_SUBDIRS = $(SUBDIRS)

libmg3d_la_SOURCES = $(COMMON_SRCS)
libmg3d_la_LDFLAGS = -release $(LT_RELEASE) -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
libmg3d_la_LIBADD = $(MG3D_LADD)

EXTRA_DIST=
