[+ AutoGen5 template cpp +]
/* $Id$
 * 
 * Copyright (c) 2001,2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2002,2003 KineIRCd Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of KineIRCd.
 * 
 * KineIRCd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * KineIRCd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with KineIRCd; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
[+(dne " * ")+]
 */[+
DEFINE read-file-as-array+][+
  (shellf "exec 4< %s; export IFS=`echo -e '\n'`" (get "file")) +][+
  WHILE `read -sr line <&4 && echo 'true'` +][+
    (sprintf "\t L%s,\n"
      (c-string (shell "echo -En $line"))) +][+
  ENDWHILE +][+
  `exec 4<&-` +][+
ENDDEF+]

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include <aisutil/aisutilconf.h>

#include "kineircd/version.h"
#include "lib/version.h"

namespace Kine {
   namespace Version {
      // Build time information
      const char* const buildTime = BUILD_TIME;
      const char* const buildTimeFull = BUILD_TIME_FULL;
      const unsigned long buildCount = BUILD_COUNT;
      
      // Source code hash
      const char* const sourceCodeHash = SOURCE_CODE_HASH;
      
      // Program name
      const char* const programName = PACKAGE;
      const char* const programNameFull = KINEIRCD_VERSION_NAME_FULL;
      
      // Program version numbers (raw)
      const unsigned short versionMajor = KINEIRCD_VERSION_MAJOR;
      const unsigned short versionMinor = KINEIRCD_VERSION_MINOR;

      // Version information (program name and version number, in its entirity)
      const char* const version = PACKAGE "-" KINEIRCD_VERSION_STRING;
      
      // Full version identity (includes build time)
      const char* const versionFull = 
	PACKAGE "-" KINEIRCD_VERSION_STRING "-" BUILD_TIME;
      
      // Library version information (raw)
      const unsigned char libVersion = KINEIRCD_LIB_VERSION;
      const unsigned char libRevision = KINEIRCD_LIB_REVISION;
      const unsigned char libAge = KINEIRCD_LIB_AGE;
      
      // E-mail address to where bug reports should wind up
      const char* const bugsEmail = PACKAGE_BUGREPORT;
      
      /* Information for INFO command (note: no colon prefixes for any 
       * protocol)
       * Note: Try to make this work OK on a 80 column display (check in text
       *       clients like EPIC/BitchX etc). Also keep this in UTF-8, or at
       *       least ISO-8859-1 encoding in a vague attempt to try and keep it 
       *       fairly standard to read for the various clients out there. This
       *       is the reason for the special formatting..
       * 
       *       PLEASE DO NOT EDIT THIS UNLESS YOU ARE TOLD TO!!! :)
       * 
       *       Warning: Under the terms of the GNU GPL you MUST NOT remove
       *       any copyright information here, nor the Kine IRC Daemon
       *       development team list (as it is an integral part of the primary
       *       copyright).
       */
      const wchar_t* const versionInfo[] = {
[+read-file-as-array file='src/lib/version.txt'+]	 0
      };
      
      /* Special version mark thingy whatsit string doohickey.. thingamejig...
       * Yes! This is ugly. Yes!! I do know about that. Yes!!! It's more 
       * efficient than any other method anyone else has been able to suggest 
       * to me so far. Yes!!!! That's right, that means I don't want your
       * comments on how #ifdef's are considered obsolete in C++! :)
       */
      const char* const versionChars =
	// Configuration characters[+FOR version_chars+]
#ifdef [+ifdef+]
				"[+char+]"
#endif[+ENDFOR+]
					; // The end, phew!
   }; // namespace Version
}; // namespace Kine

