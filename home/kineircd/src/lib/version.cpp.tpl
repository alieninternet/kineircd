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
"Portions of this code Copyright (c) 1996-2001 Alien Internet Services",
"austhex.servd8 development team:",
"  to be done.. hehe shh, it will come before release :)",
      0
   };
   
   // Version characters for VERSION command
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
#ifdef DO_MATCH_COUNTING
				"y"
#endif
#ifdef LOG_TO_SYSLOG
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
#ifdef HAVE_PROTOCOL_P13SERVER
     				",P13"
#endif
#ifdef COMPRESS_PROTOCOL_P13SERVER
     					"c"
#endif
#ifdef HAVE_PROTOCOL_P14SERVER
     				",P14"
#endif
#ifdef COMPRESS_PROTOCOL_P14SERVER
     					"c"
#endif
#ifdef HAVE_PROTOCOL_IRC2USER
     				",IRC2"
#endif
#ifdef BLOODY_IRCII_KLUGE
					"k"
#endif
#ifdef HAVE_PROTOCOL_IRC3USER
     				",IRC3"
#endif
#ifdef COMPRESS_PROTOCOL_IRC3USER
     					"c"
#endif
					; // The end, phew!
}
