/* version.cpp
 * Version and Compile information
 */

#include "autoconf.h"

#include "version.h"

namespace Version {
   // Build information
   char const *build = BUILD_STRING;

   // Version information (program name and version number)
   char const *version = PROGNAME_STRING VERSION_STRING;

   /* Information for INFO command (note: no colon prefixes for any protocol)
    * Note: Try to make this work OK on a 80 column display (check in text
    *       clients like EPIC/BitchX etc). This is the reason for the special
    *       formatting.. DO NOT EDIT THIS UNLESS YOU ARE TOLD TO!!!
    */
   char const *versionInfo[] = {
"          \026                     _   _               ___      \026",
"          \026      __ _ _   _ ___| |_| |__   _____  _( _ )     \026",
"          \026     / _` | | | / __| __|  _ \\ / _ \\ \\/ / _ \\     \026",
"          \026    | (_| | |_| \\__ | |_| | | |  __/>  < (_) |    \026",
"          \026     \\__._|\\__._|___/\\__|_| |_|\\___/_/\\_\\___/     \026",
"          \026                                                  \026",
"",
"Copyright (c) 1996,1998,1999,2000,2001,2002 Alien Internet Services",
"austhex.servd8 development team:",
"  to be done.. hehe shh, it will come before release :)",
      0
   };
   
   // Special version mark thingy whatsit string doohickey
   char const *versionChars = 
#ifdef ALLOW_LOCALOP_CONNECT
				"c"
#endif
#ifdef ALLOW_GLOBALOP_CONNECT
				"C"
#endif
#ifdef DROP_BRAINDEAD_SERVERS
				"d"
#endif
#ifdef DEBUG
				"D"
#endif
#ifdef DEBUG_EXTENDED
				"e"
#endif
#ifdef HELPERS_CAN_SEE_USER_MODES
				"h"
#endif
#ifdef STL_HAS_HASH
				"H"
#endif
#ifdef ALLOW_LOCALOP_GLOBAL_KILL
				"k"
#endif
#ifdef ALLOW_GLOBALOP_GLOBAL_KILL
				"K"
#endif
#ifdef ALLOW_LOCALOP_LOCAL_KILL
				"l"
#endif
#ifdef ALLOW_GLOBALOP_LOCAL_KILL
				"L"
#endif
#ifdef FORCE_MOTD
				"M"
#endif
#ifdef NOTIFY_ON_CMD_STATS
				"n"
#endif
#ifdef STRICT_CHANNEL_NAMES
				"N"
#endif
#ifdef PASSIVE_REGISTRATION
				"p"
#endif
#ifdef NOTIFY_PARANOID_OPERS_ON_WHOIS
				"P"
#endif
#ifdef ALLOW_LOCALOP_SQUIT
				"q"
#endif
#ifdef ALLOW_GLOBALOP_SQUIT
				"Q"
#endif
#ifdef STRICT_REGISTRATIONS
				"r"
#endif
#ifdef WARN_UNKNOWN_COMMANDS_IN_REGISTRATION
				"R"
#endif
#ifdef USER_CONNECTION_PINGPONG
				"s"
#endif
#ifdef HAVE_OPENSSL
				"S"
#endif
#ifdef CONVERT_MOTD_TABS
				"t"
#endif
#ifdef HAVE_CMD_TRACEROUTE
				"T"
#endif
#ifdef ALLOW_CLIENT_CONNECTIONS
     				"U"
#endif
#ifdef ALLOW_OPER_WALLOPS
				"w"
#endif
#ifdef ALLOW_LOCALOP_DIE
				"x"
#endif
#ifdef ALLOW_GLOBALOP_DIE
				"X"
#endif
#ifdef SYSLOG_IDENT
				"Y"
#endif
#ifdef ALLOW_SERVER_CONNECTIONS
				"z"
#endif
#ifdef ALLOW_SERVICE_CONNECTIONS
     				"Z"
#endif
#ifdef HAVE_TRANSPORT_TCP_IPV4
     				",IPv4"
#endif
#ifdef HAVE_TRANSPORT_TCP_IPV6
     				",IPv6"
#endif
#ifdef HAVE_IRC2USER_PROTOCOL
     				",IRC2"
#endif
#ifdef HAVE_IRC2USER_IRCII_KLUGE
					"i"
#endif
#ifdef HAVE_IRC2USER_MATCH_COUNTING
					"m"
#endif
#ifdef HAVE_IRC3USER_PROTOCOL
     				",IRC3"
#endif
#ifdef HAVE_P13SERVER_PROTOCOL
     				",P13"
#endif
#ifdef HAVE_P13SERVER_COMPRESS
     					"c"
#endif
#ifdef HAVE_P14SERVER_PROTOCOL
     				",P14"
#endif
#ifdef HAVE_P14SERVER_COMPRESS
     					"c"
#endif
					; // The end, phew!
}
