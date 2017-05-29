AUTOMAKE_OPTIONS = foreign

SUBDIRS = src include test m4 

# The header file installation directory  
libmg3dincludedir = $(includedir)/mg3d

# Which header files to install
libmg3dinclude_HEADERS = mg3dconfig.h

EXTRA_DIST=INSTALL README

docs:
	doxygen Doxyfile
