# $Id$
# Top source directory make rules
#

# Include our rules
include $(top_builddir)/config/automake.inc


# Sub-directories to recurse into first..
SUBDIRS=\
	lib \
	modules


# Include dirs
INCLUDES=\
	-I$(top_builddir)/include


# Binaries to build
bin_PROGRAMS=\
	ircd \
	ircpasswd


# Source files for the 'ircd' binary
ircd_SOURCES=\
	ircd.cpp
	

# Libraries for the 'ircd' binary
ircd_LDADD=\
	$(srcdir)/lib/libkineircd.la
	
	
# Source files for the 'ircpasswd' binary
ircpasswd_SOURCES=\
	ircpasswd.cpp
	
	
# Libraries for the 'ircpasswd' binary
ircpasswd_LDADD=\
	$(srcdir)/lib/libkineircd.la
	
		
