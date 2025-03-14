# $Id$
# Top make rules
#

# Include our rules
include $(srcdir)/config/automake.inc
include $(srcdir)/libopts/MakeDefs.inc



# Work out which modules we need to compile...
if KINEIRCD_COMPILE_MOD_CHANNELLOG
 KINEIRCD_TARGET_MOD_CHANNELLOG=mod_channellog.la
endif
if KINEIRCD_COMPILE_MOD_FILELOG
 KINEIRCD_TARGET_MOD_FILELOG=mod_filelog.la
endif
if KINEIRCD_COMPILE_MOD_IRC2REGISTRAR
 KINEIRCD_TARGET_MOD_IRC2REGISTRAR=mod_irc2registrar.la
endif
if KINEIRCD_COMPILE_MOD_IRC2USER
 KINEIRCD_TARGET_MOD_IRC2USER=mod_irc2user.la
endif
if KINEIRCD_COMPILE_MOD_SYSLOG
 KINEIRCD_TARGET_MOD_SYSLOG=mod_syslog.la
endif



# Libraries and modules to build
lib_LTLIBRARIES=\
	libkineircd.la \
	libkineircd_clbp.la \
	libkineircd_irc2.la \
	$(KINEIRCD_TARGET_MOD_CHANNELLOG) \
	$(KINEIRCD_TARGET_MOD_FILELOG) \
	$(KINEIRCD_TARGET_MOD_IRC2REGISTRAR) \
	$(KINEIRCD_TARGET_MOD_IRC2USER) \
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
	autogen/attributes.def \
	autogen/chanmembers.def \
	autogen/chanmodes.def \
	autogen/chantypes.def \
	autogen/config.def \
	autogen/config.scm \
	autogen/config.tpl \
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
	src/lib/version.cpp.tpl



# Stuff we don't want distributed
dist-hook:
	rm -rf $(distdir)/kineircd/kineircdconf.h


# Do we need to build libopts too?
if NEED_LIBOPTS
 LIBOPTS_SOURCE_DIR=libopts
endif


# Subdirectories to recurse into..
SUBDIRS=\
	libltdl \
	$(LIBOPTS_SOURCE_DIR) \
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
	include/kineircd/libirc2/numerics.h \
	include/kineircd/config.h \
	include/kineircd/errors.h \
	include/kineircd/events.h \
	include/kineircd/logger.h \
	include/kineircd/server.h \
	install/.lang \
	src/ircd/options.h \
	src/lib/language.h \
	src/lib/version.h \
	src/lib_irc2/language.h \
	src/mod_irc2registrar/config.h \
	src/mod_irc2registrar/language.h \
	src/mod_irc2user/commands.h \
	src/mod_irc2user/language.h \
	src/mod_irc2user/protocol.h



###############################################################################
# 'libkineircd' library
###############################################################################
if KINEIRCD_COMPILE_LIB_CONFIG_OLDCONFIG
 KINEIRCD_SOURCE_LIB_CONFIG_OLDCONFIG_CPP=src/lib/config/oldconfig.cpp
endif
if KINEIRCD_COMPILE_LIB_DEBUG
 KINEIRCD_SOURCE_LIB_DEBUG_CPP=src/lib/debug.cpp
 KINEIRCD_SOURCE_LIB_DEBUG_H=src/lib/debug.h
endif

# Includes..
libkineircd_la_INCLUDES=\
	$(LTDLINCL)

# Source/header files
libkineircd_la_SOURCES=\
	include/kineircd/protocol/base.h \
	include/kineircd/protocol/input.h \
	include/kineircd/protocol/output.h \
	include/kineircd/protocol/protocol.h \
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
	include/kineircd/mynetwork.h \
	include/kineircd/myserver.h \
	include/kineircd/network.h \
	include/kineircd/name.h \
	include/kineircd/password.h \
	include/kineircd/protocolinfo.h \
	include/kineircd/receiver.h \
	include/kineircd/registrant.h \
	include/kineircd/sender.h \
	include/kineircd/server.h \
	include/kineircd/service.h \
	include/kineircd/user.h \
	include/kineircd/version.h \
	src/lib/config/config.cpp \
	src/lib/config/handlers.cpp \
	$(KINEIRCD_SOURCE_LIB_CONFIG_OLDCONFIG_CPP) \
	src/lib/language/subst.cpp \
	src/lib/language/subst.h \
	src/lib/language/config.cpp \
	src/lib/language/config.h \
	src/lib/language/data.cpp \
	src/lib/language/interface.cpp \
	src/lib/listener/listener.cpp \
	src/lib/listener/config.cpp \
	src/lib/listener/config.h \
	src/lib/listener/list.cpp \
	src/lib/modules/descriptor.cpp \
	src/lib/modules/list.cpp \
	src/lib/channel.cpp \
	src/lib/chantypes.cpp \
	src/lib/charconv.h \
	src/lib/client.cpp \
	src/lib/connection.cpp \
	src/lib/constants.h \
	src/lib/daemon.cpp \
	$(KINEIRCD_SOURCE_LIB_DEBUG_CPP) \
	$(KINEIRCD_SOURCE_LIB_DEBUG_H) \
	src/lib/errors.cpp \
	src/lib/language.cpp \
	src/lib/language.h \
	src/lib/logger.cpp \
	src/lib/mynetwork.cpp \
	src/lib/myserver.cpp \
	src/lib/network.cpp \
	src/lib/name.cpp \
	src/lib/protocol.cpp \
	src/lib/registry.h \
	src/lib/registry.cpp \
	src/lib/server.cpp \
	src/lib/user.cpp \
	src/lib/version.cpp
	
# Flags..
libkineircd_la_LDFLAGS=\
	-version-info @LIBKINEIRCD_VERSION@:@LIBKINEIRCD_REVISION@:@LIBKINEIRCD_AGE@ \
	@LDFLAGS@ \
	@ssl_ldflags@
	
# Libraries..
libkineircd_la_LIBADD=\
	$(LIBLTDL) \
	$(LIBICONV) \
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

src/lib/config/config.cpp: src/lib/config/config.cpp.tpl $(AUTOGEN_DIR)/config.def $(AUTOGEN_DIR)/config.scm
	$(AUTOGEN) \
		-b $(basename $@) \
		-T src/lib/config/config.cpp.tpl \
		-S $(AUTOGEN_DIR)/config.scm \
		$(AUTOGEN_DIR)/config.def

src/lib/language/subst.cpp: src/lib/language/subst.cpp.tpl $(AUTOGEN_DIR)/attributes.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T src/lib/language/subst.cpp.tpl \
		$(AUTOGEN_DIR)/attributes.def

src/lib/chantypes.cpp: src/lib/chantypes.cpp.tpl $(AUTOGEN_DIR)/chantypes.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T src/lib/chantypes.cpp.tpl \
		$(AUTOGEN_DIR)/chantypes.def

src/lib/errors.cpp: src/lib/errors.cpp.tpl $(AUTOGEN_DIR)/errors.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T src/lib/errors.cpp.tpl \
		$(AUTOGEN_DIR)/errors.def

src/lib/language.cpp src/lib/language.h: $(AUTOGEN_DIR)/language.tpl $(AUTOGEN_DIR)/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T $(AUTOGEN_DIR)/language.tpl \
		-DCOMPONENT=libkineircd \
		-DNAMESPACE=Internal \
		-DSUBDIR=lib \
		$(AUTOGEN_DIR)/language.def

# Stop having to relink every module per build in maintainer mode..
# Is this wise, though?
if !MAINTAINER_MODE
.PHONY: src/lib/version.h
endif

# This is intentionally *not* in the sources list or it will fudge the checksum
src/lib/version.h:
	$(ECHO) \
		"#define SOURCE_CODE_HASH \"` \
			cat $(libkineircd_la_SOURCES) \
				| md5sum \
				| cut -d\  -f1 \
			`\"" \
		> src/lib/version.h
	$(ECHO) \
		"#define BUILD_TIME \"`date -u +%Y%m%dT%H%MZ`\"" \
		>> src/lib/version.h
	$(ECHO) \
		"#define BUILD_TIME_FULL \"`date`\"" \
		>> src/lib/version.h
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
		>> src/lib/version.h


###############################################################################
# 'libkineircd_clbp' library
###############################################################################
libkineircd_clbp_la_SOURCES=\
	include/kineircd/libclbp/input.h \
	include/kineircd/libclbp/output.h \
	include/kineircd/libclbp/protocol.h \
	src/lib_clbp/input.cpp \
	src/lib_clbp/output.cpp

libkineircd_clbp_la_LDFLAGS=\
	-version-info @LIBKINEIRCD_CLBP_VERSION@:@LIBKINEIRCD_CLBP_REVISION@:@LIBKINEIRCD_CLBP_AGE@

libkineircd_clbp_la_LIBADD=\
	libkineircd.la
		


###############################################################################
# 'libkineircd_irc2' library
###############################################################################
libkineircd_irc2_la_SOURCES=\
	include/kineircd/libirc2/input.h \
	include/kineircd/libirc2/library.h \
	include/kineircd/libirc2/numerics.h \
	include/kineircd/libirc2/output.h \
	include/kineircd/libirc2/protocol.h \
	include/kineircd/libirc2/user.h \
	include/kineircd/libirc2/utility.h \
	src/lib_irc2/common.cpp \
	src/lib_irc2/input.cpp \
	src/lib_irc2/lang.h \
	src/lib_irc2/language.cpp \
	src/lib_irc2/language.h \
	src/lib_irc2/library.cpp \
	src/lib_irc2/output.cpp \
	src/lib_irc2/stats.cpp \
	src/lib_irc2/stats.h \
	src/lib_irc2/utility.cpp

libkineircd_irc2_la_LDFLAGS=\
	-version-info @LIBKINEIRCD_IRC2_VERSION@:@LIBKINEIRCD_IRC2_REVISION@:@LIBKINEIRCD_IRC2_AGE@

libkineircd_irc2_la_LIBADD=\
	libkineircd.la \
	libkineircd_clbp.la
		
include/kineircd/libirc2/numerics.h: include/kineircd/libirc2/numerics.h.tpl $(AUTOGEN_DIR)/irc2numerics.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T include/kineircd/libirc2/numerics.h.tpl \
		$(AUTOGEN_DIR)/irc2numerics.def

src/lib_irc2/language.cpp src/lib_irc2/language.h: $(AUTOGEN_DIR)/language.tpl $(AUTOGEN_DIR)/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T $(AUTOGEN_DIR)/language.tpl \
		-DCOMPONENT=lib_irc2 \
		-DNAMESPACE=LibIRC2 \
		$(AUTOGEN_DIR)/language.def



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
# 'mod_irc2registrar' module
###############################################################################
mod_irc2registrar_la_SOURCES=\
	src/mod_irc2registrar/config.cpp \
	src/mod_irc2registrar/config.h \
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

src/mod_irc2registrar/config.cpp src/mod_irc2registrar/config.h: $(AUTOGEN_DIR)/config.tpl $(AUTOGEN_DIR)/config.def $(AUTOGEN_DIR)/config.scm
	$(AUTOGEN) \
		-b $(basename $@) \
		-T $(AUTOGEN_DIR)/config.tpl \
		-S $(AUTOGEN_DIR)/config.scm \
		-DCOMPONENT=mod_irc2registrar \
		$(AUTOGEN_DIR)/config.def

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
	$(LIBOPTS_LDADD)
	
ircd_CFLAGS=\
	$(LIBOPTS_CFLAGS)

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
