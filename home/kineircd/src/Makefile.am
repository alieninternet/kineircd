# $Id$
# Top source directory make rules
#

# Include our rules
include $(top_builddir)/config/automake.inc



# Work out which additional libraries we need to compile...
#if KINEIRCD_COMPILE_LIB_IRC2
# KINEIRCD_TARGET_LIB_IRC2=libkineircd_irc2.la
 KINEIRCD_TARGET_LIB_IRC2=
#endif

# Work out which modules we need to compile...
if KINEIRCD_COMPILE_MOD_FILELOG
 KINEIRCD_TARGET_MOD_FILELOG=mod_filelog.la
endif
if KINEIRCD_COMPILE_MOD_HTTP
 KINEIRCD_TARGET_MOD_HTTP=mod_http.la
endif
if KINEIRCD_COMPILE_MOD_IRC2USER
 KINEIRCD_TARGET_MOD_IRC2USER=mod_irc2user.la
endif
if KINEIRCD_COMPILE_MOD_SNMP
 KINEIRCD_TARGET_MOD_SNMP=mod_snmp.la
endif
if KINEIRCD_COMPILE_MOD_SYSLOG
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
libkineircd_irc2_la_SOURCES=\
	lib_irc2/parser.cpp \
	lib_irc2/protocol.h
libkineircd_irc2_la_LDFLAGS=\
	-avoid-version
libkineircd_irc2_la_LIBADD=\
	lib/libkineircd.la



###############################################################################
# 'mod_filelog' module
###############################################################################
mod_filelog_la_SOURCES=\
	mod_filelog/config.cpp \
	mod_filelog/config.h \
	mod_filelog/filelog.cpp \
	mod_filelog/filelog.h \
	mod_filelog/module.cpp \
	mod_filelog/module.h
mod_filelog_la_LDFLAGS=\
	-avoid-version \
	-module
mod_filelog_la_LIBADD=\
	lib/libkineircd.la



###############################################################################
# 'mod_http' module
###############################################################################
mod_http_la_SOURCES=\
	mod_http/module.cpp \
	mod_http/module.h
mod_http_la_LDFLAGS=\
	-avoid-version \
	-module
mod_http_la_LIBADD=\
	lib/libkineircd.la



###############################################################################
# 'mod_irc2user' module
###############################################################################
mod_irc2user_la_SOURCES=\
	mod_irc2user/module.cpp \
	mod_irc2user/module.h
mod_irc2user_la_LDFLAGS=\
	-avoid-version \
	-module
mod_irc2user_la_LIBADD=\
	lib/libkineircd.la \
	#libkineircd_irc2.la



###############################################################################
# 'mod_snmp' module
###############################################################################
mod_snmp_la_SOURCES=\
	mod_snmp/module.cpp \
	mod_snmp/module.h \
	mod_snmp/snmp.cpp \
	mod_snmp/snmp.h
mod_snmp_la_LDFLAGS=\
	-avoid-version \
	-module
mod_snmp_la_LIBADD=\
	lib/libkineircd.la



###############################################################################
# 'mod_syslog' module
###############################################################################
mod_syslog_la_SOURCES=\
	mod_syslog/module.cpp \
	mod_syslog/module.h \
	mod_syslog/syslog.cpp \
	mod_syslog/syslog.h
mod_syslog_la_LDFLAGS=\
	-avoid-version \
	-module
mod_syslog_la_LIBADD=\
	lib/libkineircd.la

# Customised .tpl to .cpp rules, since the template and the defs do not match
mod_syslog/syslog.cpp: mod_syslog/syslog.cpp.tpl $(AUTOGEN_DEF_DIR)/logger.def
	$(AUTOGEN) -b $(basename $@) -T $< $(AUTOGEN_DEF_DIR)/logger.def



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
