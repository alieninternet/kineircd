# $Id$
# Top make rules
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
if KINEIRCD_COMPILE_MOD_IRC2REGISTRAR
 KINEIRCD_TARGET_MOD_IRC2REGISTRAR=mod_irc2registrar.la
endif
if KINEIRCD_COMPILE_MOD_IRC2USER
 KINEIRCD_TARGET_MOD_IRC2USER=mod_irc2user.la
endif
if KINEIRCD_COMPILE_MOD_PALLADIUM
 KINEIRCD_TARGET_MOD_PALLADIUM=mod_palladium.la
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
	$(KINEIRCD_TARGET_MOD_IRC2REGISTRAR) \
	$(KINEIRCD_TARGET_MOD_IRC2USER) \
	$(KINEIRCD_TARGET_MOD_PALLADIUM) \
	$(KINEIRCD_TARGET_MOD_SNMP) \
	$(KINEIRCD_TARGET_MOD_SYSLOG)



# Binaries to build
bin_PROGRAMS=\
	ircd \
	ircpasswd



# The place where the include files should go
includedir=@includedir@/kineircd



# Include dirs (referential); the last one is a hack for autoopts :(
INCLUDES=\
	-I$(top_builddir)/include \
	-I$(top_builddir)/src \
	-I$(top_builddir)



# Stuff that needs to go into the distribution but isn't implied
EXTRA_DIST=\
	DEV-TEAM \
	autogen/chanmembers.def \
	autogen/chanmodes.def \
	autogen/chantypes.def \
	autogen/config.def \
	autogen/config.scm \
	autogen/errors.def \
	autogen/events.def \
	autogen/irc2numerics.def \
	autogen/language.def \
	autogen/language.tpl \
	autogen/logger.def \
	autogen/servermodes.def \
	autogen/usermodes.def \
	autogen/version.def \
	config/automake.inc \
	doc/rfc1413.txt \
	doc/rfc1459.txt \
	doc/rfc2810.txt \
	doc/rfc2811.txt \
	doc/rfc2812.txt \
	doc/rfc2813.txt \
	include/kineircd/kineircdconf.h.in \
	src/libkineircd/version.cpp.tpl



# Stuff we don't want distributed
dist-hook:
	rm -rf $(distdir)/kineircd/kineircdconf.h



# Subdirectories to recurse into..
SUBDIRS=\
	libltdl \
	.



# Customised .tpl rules
install/example.conf: install/example.conf.tpl $(AUTOGEN_DIR)/config.def $(AUTOGEN_DIR)/config.scm
	$(AUTOGEN) \
		-b $(basename $@) \
		-T install/example.conf.tpl \
		-S $(AUTOGEN_DIR)/config.scm \
		$(AUTOGEN_DIR)/config.def

install/.lang: install/language.tpl $(AUTOGEN_DIR)/language.def
	$(AUTOGEN) \
		-b install/ \
		-T install/language.tpl \
		$(AUTOGEN_DIR)/language.def



# A rule to download RFCs :)
RFC_LOCATION=http://www.rfc-editor.org/rfc
WGET=wget

rfc%.txt:
	$(WGET) \
		-N $(RFC_LOCATION)/$(notdir $@) \
		-O $@



# Pre-dependency building dependancies...
BUILT_SOURCES=\
	include/kineircd/irc2/numerics.h \
	include/kineircd/config.h \
	include/kineircd/errors.h \
	include/kineircd/events.h \
	include/kineircd/logger.h \
	include/kineircd/server.h \
	install/.lang \
	src/ircd/options.h \
	src/libkineircd_irc2/language.h \
	src/mod_irc2registrar/language.h \
	src/mod_irc2user/commands.h \
	src/mod_irc2user/language.h \
	src/mod_irc2user/protocol.h



###############################################################################
# 'libkineircd' library
###############################################################################
if KINEIRCD_COMPILE_LIB_CONFIG_OLDCONFIG
 KINEIRCD_SOURCE_LIB_CONFIG_OLDCONFIG_CPP=src/libkineircd/config/oldconfig.cpp
endif
if KINEIRCD_COMPILE_LIB_DEBUG
 KINEIRCD_SOURCE_LIB_DEBUG_CPP=src/libkineircd/debug.cpp
 KINEIRCD_SOURCE_LIB_DEBUG_H=src/libkineircd/debug.h
endif

# Includes..
libkineircd_la_INCLUDES=\
	$(LTDLINCL)

# Source/header files
libkineircd_la_SOURCES=\
	include/kineircd/channel.h \
	include/kineircd/client.h \
	include/kineircd/config.h \
	include/kineircd/connection.h \
	include/kineircd/daemon.h \
	include/kineircd/denizen.h \
	include/kineircd/entity.h \
	include/kineircd/errors.h \
	include/kineircd/events.h \
	include/kineircd/kineircdconf.h \
	include/kineircd/languages.h \
	include/kineircd/listener.h \
	include/kineircd/listenerlist.h \
	include/kineircd/localuser.h \
	include/kineircd/logger.h \
	include/kineircd/module.h \
	include/kineircd/moduledescriptor.h \
	include/kineircd/modulelist.h \
	include/kineircd/network.h \
	include/kineircd/name.h \
	include/kineircd/password.h \
	include/kineircd/protocol.h \
	include/kineircd/protocolinfo.h \
	include/kineircd/receiver.h \
	include/kineircd/registrant.h \
	include/kineircd/registry.h \
	include/kineircd/sender.h \
	include/kineircd/server.h \
	include/kineircd/service.h \
	include/kineircd/signals.h \
	include/kineircd/todo.h \
	include/kineircd/user.h \
	include/kineircd/version.h \
	src/libkineircd/config/config.cpp \
	src/libkineircd/config/handlers.cpp \
	$(KINEIRCD_SOURCE_LIB_CONFIG_OLDCONFIG_CPP) \
	src/libkineircd/language/config.cpp \
	src/libkineircd/language/config.h \
	src/libkineircd/language/data.cpp \
	src/libkineircd/language/interface.cpp \
	src/libkineircd/listener/listener.cpp \
	src/libkineircd/listener/config.cpp \
	src/libkineircd/listener/config.h \
	src/libkineircd/listener/list.cpp \
	src/libkineircd/modules/modules.cpp \
	src/libkineircd/modules/descriptor.cpp \
	src/libkineircd/modules/list.cpp \
	src/libkineircd/connection.cpp \
	src/libkineircd/daemon.cpp \
	$(KINEIRCD_SOURCE_LIB_DEBUG_CPP) \
	$(KINEIRCD_SOURCE_LIB_DEBUG_H) \
	src/libkineircd/logger.cpp \
	src/libkineircd/name.cpp \
	src/libkineircd/registry.cpp \
	src/libkineircd/signals.cpp \
	src/libkineircd/todo.cpp \
	src/libkineircd/user.cpp \
	src/libkineircd/version.cpp
	
# Flags..
libkineircd_la_LDFLAGS=\
	-version-info @LIBKINEIRCD_VERSION@:@LIBKINEIRCD_REVISION@:@LIBKINEIRCD_AGE@ \
	@LDFLAGS@ \
	@ssl_ldflags@
	
# Libraries..
libkineircd_la_LIBADD=\
	$(LIBLTDL) \
	-laisutil \
	@ssl_lib@

# Customised .tpl rules
include/kineircd/config.h: include/kineircd/config.h.tpl $(AUTOGEN_DIR)/config.def $(AUTOGEN_DIR)/config.scm
	$(AUTOGEN) \
		-b $(basename $@) \
		-T include/kineircd/config.h.tpl \
		-S $(AUTOGEN_DIR)/config.scm \
		$(AUTOGEN_DIR)/config.def

include/kineircd/server.h: include/kineircd/server.h.tpl $(AUTOGEN_DIR)/servermodes.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T include/kineircd/server.h.tpl \
		$(AUTOGEN_DIR)/servermodes.def

src/libkineircd/config/config.cpp: src/libkineircd/config/config.cpp.tpl $(AUTOGEN_DIR)/config.def $(AUTOGEN_DIR)/config.scm
	$(AUTOGEN) \
		-b $(basename $@) \
		-T src/libkineircd/config/config.cpp.tpl \
		-S $(AUTOGEN_DIR)/config.scm \
		$(AUTOGEN_DIR)/config.def

# This is intentionally *not* in the sources list or it will fudge the checksum
.PHONY: src/libkineircd/version.h
src/libkineircd/version.h:
	$(ECHO) \
		"#define SOURCE_CODE_HASH \"` \
			cat $(libkineircd_la_SOURCES) \
				| md5sum \
				| cut -d\  -f1 \
			`\"" \
		> src/libkineircd/version.h
	$(ECHO) \
		"#define BUILD_TIME \"`date -u +%Y%m%dT%H%MZ`\"" \
		>> src/libkineircd/version.h
	$(ECHO) \
		"#define BUILD_TIME_FULL \"`date`\"" \
		>> src/libkineircd/version.h
	if \
		test -e $(top_builddir)/.builds; then \
			expr \
				`cat $(top_builddir)/.builds` \
				+ 1 \
				> $(top_builddir)/.builds; \
		else \
			echo "1" > $(top_builddir)/.builds; \
		fi
	$(ECHO) \
		"#define BUILD_COUNT `cat $(top_builddir)/.builds`" \
		>> src/libkineircd/version.h


###############################################################################
# 'libkineircd_irc2' library
###############################################################################
libkineircd_irc2_la_SOURCES=\
	include/kineircd/irc2/library.h \
	include/kineircd/irc2/modes.h \
	include/kineircd/irc2/numerics.h \
	include/kineircd/irc2/protocol.h \
	include/kineircd/irc2/utility.h \
	src/libkineircd_irc2/common.cpp \
	src/libkineircd_irc2/lang.h \
	src/libkineircd_irc2/language.cpp \
	src/libkineircd_irc2/language.h \
	src/libkineircd_irc2/library.cpp \
	src/libkineircd_irc2/protocol.cpp \
	src/libkineircd_irc2/servermodes.cpp \
	src/libkineircd_irc2/stats.cpp \
	src/libkineircd_irc2/stats.h \
	src/libkineircd_irc2/utility.cpp

libkineircd_irc2_la_LDFLAGS=\
	-version-info @LIBKINEIRCD_IRC2_VERSION@:@LIBKINEIRCD_IRC2_REVISION@:@LIBKINEIRCD_IRC2_AGE@

libkineircd_irc2_la_LIBADD=\
	libkineircd.la
		
include/kineircd/irc2/numerics.h: include/kineircd/irc2/numerics.h.tpl $(AUTOGEN_DIR)/irc2numerics.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T include/kineircd/irc2/numerics.h.tpl \
		$(AUTOGEN_DIR)/irc2numerics.def

src/libkineircd_irc2/language.cpp src/libkineircd_irc2/language.h: $(AUTOGEN_DIR)/language.tpl $(AUTOGEN_DIR)/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T $(AUTOGEN_DIR)/language.tpl \
		-DCOMPONENT=libkineircd_irc2 \
		-DNAMESPACE=LibIRC2 \
		$(AUTOGEN_DIR)/language.def



###############################################################################
# 'mod_bovini1' module
###############################################################################
mod_bovini1_la_SOURCES=\
	src/mod_bovini1/module.cpp \
	src/mod_bovini1/module.h

mod_bovini1_la_LDFLAGS=\
	-avoid-version \
	-module

mod_bovini1_la_LIBADD=\
	libkineircd.la



###############################################################################
# 'mod_channellog' module
###############################################################################
mod_channellog_la_SOURCES=\
	src/mod_channellog/channellog.cpp \
	src/mod_channellog/channellog.h \
	src/mod_channellog/config.cpp \
	src/mod_channellog/config.h \
	src/mod_channellog/module.cpp \
	src/mod_channellog/module.h

mod_channellog_la_LDFLAGS=\
	-avoid-version \
	-module

mod_channellog_la_LIBADD=\
	libkineircd.la



###############################################################################
# 'mod_filelog' module
###############################################################################
mod_filelog_la_SOURCES=\
	src/mod_filelog/config.cpp \
	src/mod_filelog/config.h \
	src/mod_filelog/filelog.cpp \
	src/mod_filelog/filelog.h \
	src/mod_filelog/module.cpp \
	src/mod_filelog/module.h

mod_filelog_la_LDFLAGS=\
	-avoid-version \
	-module

mod_filelog_la_LIBADD=\
	libkineircd.la



###############################################################################
# 'mod_http' module
###############################################################################
mod_http_la_SOURCES=\
	src/mod_http/module.cpp \
	src/mod_http/module.h

mod_http_la_LDFLAGS=\
	-avoid-version \
	-module

mod_http_la_LIBADD=\
	libkineircd.la



###############################################################################
# 'mod_iirc1' module
###############################################################################
mod_iirc1_la_SOURCES=\
	src/mod_iirc1/module.cpp \
	src/mod_iirc1/module.h

mod_iirc1_la_LDFLAGS=\
	-avoid-version \
	-module

mod_iirc1_la_LIBADD=\
	libkineircd.la \
	libkineircd_irc2.la



###############################################################################
# 'mod_irc2registrar' module
###############################################################################
mod_irc2registrar_la_SOURCES=\
	src/mod_irc2registrar/debug.h \
	src/mod_irc2registrar/language.cpp \
	src/mod_irc2registrar/language.h \
	src/mod_irc2registrar/module.cpp \
	src/mod_irc2registrar/module.h \
	src/mod_irc2registrar/protocol.cpp \
	src/mod_irc2registrar/protocol.h \
	src/mod_irc2registrar/protocolinfo.h

mod_irc2registrar_la_LDFLAGS=\
	-avoid-version \
	-module

mod_irc2registrar_la_LIBADD=\
	libkineircd.la \
	libkineircd_irc2.la

src/mod_irc2registrar/language.cpp src/mod_irc2registrar/language.h: $(AUTOGEN_DIR)/language.tpl $(AUTOGEN_DIR)/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T $(AUTOGEN_DIR)/language.tpl \
		-DCOMPONENT=mod_irc2registrar \
		$(AUTOGEN_DIR)/language.def



###############################################################################
# 'mod_irc2user' module
###############################################################################
# Extra flags and stuff
if KINEIRCD_MOD_IRC2USER_WITH_SCHEME
 KINEIRCD_MOD_IRC2USER_SCHEME_CFLAGS=@GUILE_CFLAGS@
 KINEIRCD_MOD_IRC2USER_SCHEME_LDFLAGS=@GUILE_LDFLAGS@
 KINEIRCD_MOD_IRC2USER_SCHEME_SOURCES=\
	src/mod_irc2user/numerics.cpp \
	src/mod_irc2user/scheme.cpp \
	src/mod_irc2user/scheme.h \
	src/mod_irc2user/schemesubs.cpp \
	src/mod_irc2user/schemesubs.h
endif


mod_irc2user_la_SOURCES=\
	src/mod_irc2user/commandlist.cpp \
	src/mod_irc2user/commands.cpp \
	src/mod_irc2user/commands.h \
	src/mod_irc2user/handlers.cpp \
	src/mod_irc2user/isupport.cpp \
	src/mod_irc2user/isupport.h \
	src/mod_irc2user/lang.h \
	src/mod_irc2user/language.cpp \
	src/mod_irc2user/language.h \
	src/mod_irc2user/messaging.cpp \
	src/mod_irc2user/module.cpp \
	src/mod_irc2user/module.h \
	src/mod_irc2user/protocol.cpp \
	src/mod_irc2user/protocol.h \
	src/mod_irc2user/protocolinfo.h \
	src/mod_irc2user/user.cpp \
	src/mod_irc2user/user.h \
	$(KINEIRCD_MOD_IRC2USER_SCHEME_SOURCES)

mod_irc2user_la_LDFLAGS=\
	-avoid-version \
	-module \
	$(KINEIRCD_MOD_IRC2USER_SCHEME_LDFLAGS)

mod_irc2user_la_LIBADD=\
	libkineircd.la \
	libkineircd_irc2.la

# I do not like how automake manages this..
mod_irc2user_la_CFLAGS=\
	$(KINEIRCD_MOD_IRC2USER_SCHEME_CFLAGS)

src/mod_irc2user/commandlist.cpp: src/mod_irc2user/commandlist.cpp.tpl src/mod_irc2user/commands.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T src/mod_irc2user/commandlist.cpp.tpl \
		src/mod_irc2user/commands.def

src/mod_irc2user/commands.h: src/mod_irc2user/commands.h.tpl src/mod_irc2user/commands.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T src/mod_irc2user/commands.h.tpl \
		src/mod_irc2user/commands.def

src/mod_irc2user/language.cpp src/mod_irc2user/language.h: $(AUTOGEN_DIR)/language.tpl $(AUTOGEN_DIR)/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T $(AUTOGEN_DIR)/language.tpl \
		-DCOMPONENT=mod_irc2user \
		$(AUTOGEN_DIR)/language.def

src/mod_irc2user/numerics.cpp: src/mod_irc2user/numerics.cpp.tpl $(AUTOGEN_DIR)/irc2numerics.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T src/mod_irc2user/numerics.cpp.tpl \
		$(AUTOGEN_DIR)/irc2numerics.def

src/mod_irc2user/protocol.h: src/mod_irc2user/protocol.h.tpl src/mod_irc2user/commands.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T src/mod_irc2user/protocol.h.tpl \
		src/mod_irc2user/commands.def

src/mod_irc2user/schemesubs.cpp src/mod_irc2user/schemesubs.h: src/mod_irc2user/schemesubs.tpl src/mod_irc2user/schemesubs.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T src/mod_irc2user/schemesubs.tpl \
		src/mod_irc2user/schemesubs.def



###############################################################################
# 'mod_palladium' module
###############################################################################
mod_palladium_la_SOURCES=\
	src/mod_palladium/ctcp.cpp \
	src/mod_palladium/module.cpp \
	src/mod_palladium/module.h \
	src/mod_palladium/service.cpp \
	src/mod_palladium/service.h

mod_palladium_la_LDFLAGS=\
	-avoid-version \
	-module

mod_palladium_la_LIBADD=\
	libkineircd.la



###############################################################################
# 'mod_snmp' module
###############################################################################
mod_snmp_la_SOURCES=\
	src/mod_snmp/module.cpp \
	src/mod_snmp/module.h \
	src/mod_snmp/snmp.cpp \
	src/mod_snmp/snmp.h

mod_snmp_la_LDFLAGS=\
	-avoid-version \
	-module

mod_snmp_la_LIBADD=\
	libkineircd.la



###############################################################################
# 'mod_syslog' module
###############################################################################
mod_syslog_la_SOURCES=\
	src/mod_syslog/config.cpp \
	src/mod_syslog/config.h \
	src/mod_syslog/module.cpp \
	src/mod_syslog/module.h \
	src/mod_syslog/syslog.cpp \
	src/mod_syslog/syslog.h

mod_syslog_la_LDFLAGS=\
	-avoid-version \
	-module

mod_syslog_la_LIBADD=\
	libkineircd.la

# Customised .tpl to .cpp rules
src/mod_syslog/syslog.cpp: src/mod_syslog/syslog.cpp.tpl $(AUTOGEN_DIR)/logger.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T src/mod_syslog/syslog.cpp.tpl \
		$(AUTOGEN_DIR)/logger.def



###############################################################################
# 'ircd' binary
###############################################################################
ircd_SOURCES=\
	src/ircd/bits.cpp \
	src/ircd/bits.h \
	src/ircd/exit.h \
	src/ircd/ircd.cpp \
	src/ircd/options.c \
	src/ircd/options.h

ircd_LDADD=\
	libkineircd.la \
	-lopts
	
src/ircd/options.c src/ircd/options.h: src/ircd/options.def
	$(AUTOGEN) \
		-b $(basename $@) \
		src/ircd/options.def



###############################################################################
# 'ircpasswd' binary
###############################################################################
ircpasswd_SOURCES=\
	src/utils/ircpasswd.cpp

ircpasswd_LDADD=\
	libkineircd.la
