/* version.cpp
 * Version and Compile information
 */

#include "version.h"
#include "config.h"


// Strings
char const *getBuild = BUILD_STRING;

char const *getVersion = PROGNAME_STRING VERSION_STRING;

char const *getVersionChars = 
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
#ifdef BLOODY_IRCII_KLUGE
				"I"
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
#ifdef SHORT_MOTD
				"m"
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
#ifdef ALLOW_OPER_TRACEROUTE
				"T"
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
#ifdef LOG_TO_SYSLOG
				"Y"
#endif
#ifdef DO_MATCH_COUNTING
				"z"
#endif
					; // The end, phew!

