# $Id$
# Top source directory make rules
#

# Include our rules
include $(top_builddir)/config/automake.inc



# Work out which additional libraries we need to compile...
#if KINEIRCD_COMPILE_MODULES_LIB_IRC2
# KINEIRCD_TARGET_LIB_IRC2=libkineircd_irc2.la
 KINEIRCD_TARGET_LIB_IRC2=
#endif

# Work out which modules we need to compile...
if KINEIRCD_COMPILE_MODULES_MOD_FILELOG
 KINEIRCD_TARGET_MOD_FILELOG=mod_filelog.la
endif
if KINEIRCD_COMPILE_MODULES_MOD_HTTP
 KINEIRCD_TARGET_MOD_HTTP=mod_http.la
endif
if KINEIRCD_COMPILE_MODULES_MOD_IRC2USER
 KINEIRCD_TARGET_MOD_IRC2USER=mod_irc2user.la
endif
if KINEIRCD_COMPILE_MODULES_MOD_SNMP
 KINEIRCD_TARGET_MOD_SNMP=mod_snmp.la
endif
if KINEIRCD_COMPILE_MODULES_MOD_SYSLOG
 KINEIRCD_TARGET_MOD_SYSLOG=mod_syslog.la
endif




# Sub-directories to recurse into first..
SUBDIRS=\
	lib



# Libraries and modules to build
lib_LTLIBRARIES=\
	$(KINEIRCD_TARGET_LIB_IRC2) \
	$(KINEIRCD_TARGET_MOD_FILELOG) \
	$(KINEIRCD_TARGET_MOD_HTTP) \
	$(KINEIRCD_TARGET_MOD_IRC2USER) \
	$(KINEIRCD_TARGET_MOD_SNMP) \
	$(KINEIRCD_TARGET_MOD_SYSLOG)



# Binaries to build
bin_PROGRAMS=\
	ircd \
	ircpasswd



# Include dirs
INCLUDES=\
	-I$(top_builddir)/include \
	-I$(top_builddir)/src



###############################################################################
# 'libkineircd_irc2' library
###############################################################################
noinst_libkineircd_irc2_la_HEADERS=\
	lib_irc2/protocol.h
libkineircd_irc2_la_SOURCES=\
	lib_irc2/parser.cpp
libkineircd_irc2_la_LDFLAGS=\
	-avoid-version
libkineircd_irc2_la_LIBADD=\
	lib/libkineircd.la



###############################################################################
# 'mod_filelog' module
###############################################################################
noinst_mod_filelog_la_HEADERS=\
	mod_filelog/file.h \
	mod_filelog/module.h
mod_filelog_la_SOURCES=\
	mod_filelog/module.cpp
mod_filelog_la_LDFLAGS=\
	-avoid-version \
	-module
mod_filelog_la_LIBADD=\
	lib/libkineircd.la



###############################################################################
# 'mod_http' module
###############################################################################
noinst_mod_http_la_HEADERS=\
	mod_http/module.h
mod_http_la_SOURCES=\
	mod_http/module.cpp
mod_http_la_LDFLAGS=\
	-avoid-version \
	-module
mod_http_la_LIBADD=\
	lib/libkineircd.la



###############################################################################
# 'mod_irc2user' module
###############################################################################
noinst_mod_irc2user_la_HEADERS=\
	mod_irc2user/module.h
mod_irc2user_la_SOURCES=\
	mod_irc2user/module.cpp
mod_irc2user_la_LDFLAGS=\
	-avoid-version \
	-module
mod_irc2user_la_LIBADD=\
	lib/libkineircd.la \
	#libkineircd_irc2.la



###############################################################################
# 'mod_snmp' module
###############################################################################
noinst_mod_snmp_la_HEADERS=\
	mod_snmp/module.h
mod_snmp_la_SOURCES=\
	mod_snmp/module.cpp \
	mod_snmp/snmp.cpp
mod_snmp_la_LDFLAGS=\
	-avoid-version \
	-module
mod_snmp_la_LIBADD=\
	lib/libkineircd.la



###############################################################################
# 'mod_syslog' module
###############################################################################
noinst_mod_syslog_la_HEADERS=\
	mod_syslog/syslog.h \
	mod_syslog/module.h
mod_syslog_la_SOURCES=\
	mod_syslog/module.cpp
mod_syslog_la_LDFLAGS=\
	-avoid-version \
	-module
mod_syslog_la_LIBADD=\
	lib/libkineircd.la



###############################################################################
# 'ircd' binary
###############################################################################
ircd_SOURCES=\
	ircd.cpp
ircd_LDADD=\
	lib/libkineircd.la



###############################################################################
# 'ircpasswd' binary
###############################################################################
ircpasswd_SOURCES=\
	ircpasswd.cpp
ircpasswd_LDADD=\
	lib/libkineircd.la
