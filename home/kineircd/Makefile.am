# $Id$
# Top make rules
#

SUBDIRS=\
	include \
	src \
	install


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
	doc/rfc2813.txt
	

# A rule to download RFCs :)
RFC_LOCATION=http://www.rfc-editor.org/rfc
WGET=wget

rfc%.txt:
	$(WGET) -N $(RFC_LOCATION)/$(notdir $@) -O $@
