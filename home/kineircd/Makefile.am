# $Id$
# Top make rules
#

# Include our rules
include $(top_builddir)/config/automake.inc



# Subdirectories we need to compile...
SUBDIRS=.\
	src \
	install



# The place where the include files should go
includedir=@includedir@/kineircd



# KineIRCd library header files
include_HEADERS=\
	include/kineircd/channel.h \
	include/kineircd/client.h \
	include/kineircd/config.h \
	include/kineircd/connection.h \
	include/kineircd/daemon.h \
	include/kineircd/errors.h \
	include/kineircd/exit.h \
	include/kineircd/kineircdconf.h \
	include/kineircd/languagedata.h \
	include/kineircd/languagelist.h \
	include/kineircd/listener.h \
	include/kineircd/listenerlist.h \
	include/kineircd/logger.h \
	include/kineircd/loggerconfig.h \
	include/kineircd/module.h \
	include/kineircd/moduledescriptor.h \
	include/kineircd/modulelist.h \
	include/kineircd/network.h \
	include/kineircd/password.h \
	include/kineircd/protocol.h \
	include/kineircd/protocolinfo.h \
	include/kineircd/receiver.h \
	include/kineircd/sender.h \
	include/kineircd/server.h \
	include/kineircd/servermodes.h \
	include/kineircd/service.h \
	include/kineircd/signals.h \
	include/kineircd/str.h \
	include/kineircd/user.h \
	include/kineircd/utils.h \
	include/kineircd/version.h



# Stuff that needs to go into the distribution but isn't implied
EXTRA_DIST=\
	DEV-TEAM \
	autogen/chanmembers.def \
	autogen/chanmodes.def \
	autogen/chantypes.def \
	autogen/errors.def \
	autogen/irc2numerics.def \
	autogen/logger.def \
	autogen/servermodes.def \
	autogen/usermodes.def \
	config/automake.inc \
	doc/rfc1413.txt \
	doc/rfc1459.txt \
	doc/rfc2810.txt \
	doc/rfc2811.txt \
	doc/rfc2812.txt \
	doc/rfc2813.txt \
	include/kineircd/kineircdconf.h.in
	


# Stuff we don't want distributed
dist-hook:
	rm -rf $(distdir)/kineircd/kineircdconf.h



# A rule to download RFCs :)
RFC_LOCATION=http://www.rfc-editor.org/rfc
WGET=wget

rfc%.txt:
	$(WGET) -N $(RFC_LOCATION)/$(notdir $@) -O $@
