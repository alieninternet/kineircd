/* version.cpp
 * Version and Compile information
 * 
 * Copyright (c) 2001,2002 AustHex Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of AustHex.
 * 
 * AustHex is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AustHex is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with AustHex; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "autoconf.h"

#include "version.h"

namespace Version {
   // Build information
   char const *build = BUILD_STRING;

   // Program name
   char const *programName = PACKAGE_NAME;
   
   // Program version numbers (raw)
   unsigned short versionMajor = VERSION_MAJOR;
   unsigned short versionMinor = VERSION_MINOR;

   // Version information (program name and version number, in its entirity)
   char const *version = PACKAGE_NAME PACKAGE_VERSION;

   // Full version identity (includes build number)
   char const *versionFull = PACKAGE_NAME PACKAGE_VERSION "#" BUILD_STRING;
   
   // E-mail address to where bug reports should wind up
   char const *bugsEmail = PACKAGE_BUGREPORT;
   
   /* Information for INFO command (note: no colon prefixes for any protocol)
    * Note: Try to make this work OK on a 80 column display (check in text
    *       clients like EPIC/BitchX etc). Also keep this in UTF-8, or at
    *       least ISO-8859-1 encoding in a vague attempt to try and keep it 
    *       fairly standard to read for the various clients out there. This
    *       is the reason for the special formatting..
    *       PLEASE DO NOT EDIT THIS UNLESS YOU ARE TOLD TO!!! :)
    */
   char const *versionInfo[] = {
"          \026                     _   _               ___      \026",
"          \026      __ _ _   _ ___| |_| |__   _____  _( _ )     \026",
"          \026     / _` | | | / __| __|  _ \\ / _ \\ \\/ / _ \\     \026",
"          \026    | (_| | |_| \\__ | |_| | | |  __/>  < (_) |    \026",
"          \026     \\__._|\\__._|___/\\__|_| |_|\\___/_/\\_\\___/     \026",
"          \026                                                  \026",
"",
"Copyright \251 1996,1998,1999,2000,2001,2002 Alien Internet Services",
"Copyright \251 2001,2002 AustHex Development Team:",       
"  to be done.. hehe shh, it will come before release :)",
      0
   };
   
   /* Special version mark thingy whatsit string doohickey.. thingamejig...
    * Yes! This is ugly. Yes!! I do know about that. Yes!!! It's more efficient
    * than any other method anyone else has been able to suggest to me
    * so far. Yes!!!! That's right, that means I don't want your comments on
    * how #ifdef's are considered obsolete in C++! :)
    */
   char const *versionChars =
// Section 1 - Configuration characters     
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
     
// Section 2 - Transport protocol phrases
#ifdef HAVE_TRANSPORT_TCP_IPV4
     				",IPv4"
#endif
#ifdef HAVE_TRANSPORT_TCP_IPV6
     				",IPv6"
#endif
     
// Section 3 - IRC protocol phrases
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
#ifdef HAVE_P13SERVER_EXTENDED
     					"e"
#endif
#ifdef HAVE_P14SERVER_PROTOCOL
     				",P14"
#endif
#ifdef HAVE_P14SERVER_COMPRESS
     					"c"
#endif
					; // The end, phew!
}
