# $Id$
# Top source directory make rules
#

# Include our rules
include $(top_builddir)/config/automake.inc



# Work out which modules we need to compile...
if KINEIRCD_COMPILE_MOD_BOVINI1
 KINEIRCD_TARGET_MOD_BOVINI1=mod_bovini1.la
endif
if KINEIRCD_COMPILE_MOD_CHANNELLOG
 KINEIRCD_TARGET_MOD_CHANNELLOG=mod_channellog.la
endif
if KINEIRCD_COMPILE_MOD_FILELOG
 KINEIRCD_TARGET_MOD_FILELOG=mod_filelog.la
endif
if KINEIRCD_COMPILE_MOD_HTTP
 KINEIRCD_TARGET_MOD_HTTP=mod_http.la
endif
if KINEIRCD_COMPILE_MOD_IIRC1
 KINEIRCD_TARGET_MOD_IIRC1=mod_iirc1.la
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




# Libraries and modules to build
lib_LTLIBRARIES=\
	libkineircd.la \
	libkineircd_irc2.la \
	$(KINEIRCD_TARGET_MOD_BOVINI1) \
	$(KINEIRCD_TARGET_MOD_CHANNELLOG) \
	$(KINEIRCD_TARGET_MOD_FILELOG) \
	$(KINEIRCD_TARGET_MOD_HTTP) \
	$(KINEIRCD_TARGET_MOD_IIRC1) \
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



# Stuff that needs to go into the distribution but isn't implied
EXTRA_DIST=\
	libkineircd/loggerconfig.cpp.tpl \
	libkineircd/regnumerics.h.tpl \
	libkineircd/version.cpp.tpl



###############################################################################
# 'libkineircd' library
###############################################################################
if KINEIRCD_COMPILE_LIB_CONFIG_OLDCONFIG
 KINEIRCD_SOURCE_LIB_CONFIG_OLDCONFIG_CPP=libkineircd/config/oldconfig.cpp
endif
if KINEIRCD_COMPILE_LIB_DEBUG
 KINEIRCD_SOURCE_LIB_DEBUG_CPP=libkineircd/debug.cpp
 KINEIRCD_SOURCE_LIB_DEBUG_H=libkineircd/debug.h
endif

# Stuff we really need :(
BUILT_SOURCES=\
	libkineircd/regnumerics.h

libkineircd_la_SOURCES=\
	libkineircd/config/config.cpp \
	libkineircd/config/defaults.h \
	libkineircd/config/handlers.cpp \
	$(KINEIRCD_SOURCE_LIB_CONFIG_OLDCONFIG_CPP) \
	libkineircd/language/config.cpp \
	libkineircd/language/config.h \
	libkineircd/language/data.cpp \
	libkineircd/language/list.cpp \
	libkineircd/listener/listener.cpp \
	libkineircd/listener/config.cpp \
	libkineircd/listener/config.h \
	libkineircd/listener/list.cpp \
	libkineircd/modules/modules.cpp \
	libkineircd/modules/descriptor.cpp \
	libkineircd/modules/list.cpp \
	libkineircd/connection.cpp \
	libkineircd/daemon.cpp \
	$(KINEIRCD_SOURCE_LIB_DEBUG_CPP) \
	$(KINEIRCD_SOURCE_LIB_DEBUG_H) \
	libkineircd/logger.cpp \
	libkineircd/loggerconfig.cpp \
	libkineircd/registrar.cpp \
	libkineircd/registrar.h \
	libkineircd/sha1.cpp \
	libkineircd/signals.cpp \
	libkineircd/utils.cpp \
	libkineircd/version.cpp
libkineircd_la_LDFLAGS=\
	@LDFLAGS@ \
	@ssl_ldflags@
libkineircd_la_LIBADD=\
	libkineircd_irc2.la \
	-ldl \
	-laisutil \
	@ssl_lib@

# Customised .tpl to .cpp rules
libkineircd/regnumerics.h: libkineircd/regnumerics.h.tpl $(AUTOGEN_DEF_DIR)/irc2numerics.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T $< \
		$(AUTOGEN_DEF_DIR)/irc2numerics.def

libkineircd/loggerconfig.cpp: libkineircd/loggerconfig.cpp.tpl $(AUTOGEN_DEF_DIR)/logger.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T $< \
		$(AUTOGEN_DEF_DIR)/logger.def
		
libkineircd/config/config.cpp: libkineircd/config/config.cpp.tpl $(AUTOGEN_DEF_DIR)/config.def $(AUTOGEN_DEF_DIR)/config.scm
	$(AUTOGEN) \
		-b $(basename $@) \
		-T $< \
		-S $(AUTOGEN_DEF_DIR)/config.scm \
		$(AUTOGEN_DEF_DIR)/logger.def
		


###############################################################################
# 'libkineircd_irc2' library
###############################################################################
libkineircd_irc2_la_SOURCES=\
	libkineircd_irc2/modes.h \
	libkineircd_irc2/protocol.cpp \
	libkineircd_irc2/protocol.h \
	libkineircd_irc2/servermodes.cpp

libkineircd_irc2_la_LDFLAGS=\
	-avoid-version

libkineircd_irc2_la_LIBADD=\
	# libkineircd.la



###############################################################################
# 'mod_bovini1' module
###############################################################################
mod_bovini1_la_SOURCES=\
	mod_bovini1/module.cpp \
	mod_bovini1/module.h

mod_bovini1_la_LDFLAGS=\
	-avoid-version \
	-module

mod_bovini1_la_LIBADD=\
	libkineircd.la



###############################################################################
# 'mod_channellog' module
###############################################################################
mod_channellog_la_SOURCES=\
	mod_channellog/channellog.cpp \
	mod_channellog/channellog.h \
	mod_channellog/config.cpp \
	mod_channellog/config.h \
	mod_channellog/module.cpp \
	mod_channellog/module.h

mod_channellog_la_LDFLAGS=\
	-avoid-version \
	-module

mod_channellog_la_LIBADD=\
	libkineircd.la



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
	libkineircd.la



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
	libkineircd.la



###############################################################################
# 'mod_iirc1' module
###############################################################################
mod_iirc1_la_SOURCES=\
	mod_iirc1/module.cpp \
	mod_iirc1/module.h

mod_iirc1_la_LDFLAGS=\
	-avoid-version \
	-module

mod_iirc1_la_LIBADD=\
	libkineircd.la \
	libkineircd_irc2.la



###############################################################################
# 'mod_irc2user' module
###############################################################################
mod_irc2user_la_SOURCES=\
	mod_irc2user/module.cpp \
	mod_irc2user/module.h \
	mod_irc2user/protocol.cpp \
	mod_irc2user/protocol.h \
	mod_irc2user/protocolinfo.h

mod_irc2user_la_LDFLAGS=\
	-avoid-version \
	-module

mod_irc2user_la_LIBADD=\
	libkineircd.la \
	libkineircd_irc2.la



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
	libkineircd.la



###############################################################################
# 'mod_syslog' module
###############################################################################
mod_syslog_la_SOURCES=\
	mod_syslog/config.cpp \
	mod_syslog/config.h \
	mod_syslog/module.cpp \
	mod_syslog/module.h \
	mod_syslog/syslog.cpp \
	mod_syslog/syslog.h

mod_syslog_la_LDFLAGS=\
	-avoid-version \
	-module

mod_syslog_la_LIBADD=\
	libkineircd.la

# Customised .tpl to .cpp rules
mod_syslog/syslog.cpp: mod_syslog/syslog.cpp.tpl $(AUTOGEN_DEF_DIR)/logger.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T $< \
		$(AUTOGEN_DEF_DIR)/logger.def



###############################################################################
# 'ircd' binary
###############################################################################
ircd_SOURCES=\
	ircd.cpp

ircd_LDADD=\
	libkineircd.la



###############################################################################
# 'ircpasswd' binary
###############################################################################
ircpasswd_SOURCES=\
	ircpasswd.cpp

ircpasswd_LDADD=\
	libkineircd.la
