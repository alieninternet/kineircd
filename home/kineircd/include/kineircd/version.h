/* $Id$
 * Version and Compile information
 * 
 * Copyright (c) 2001,2002 KineIRCd Development Team
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
 */

#ifndef _INCLUDE_KINEIRCD_VERSION_H_
# define _INCLUDE_KINEIRCD_VERSION_H_

namespace Kine {
   namespace Version {
      extern const char *build;				// Build information
      extern const char *programName;			// Program name
      extern const unsigned short versionMajor;		// Version major number
      extern const unsigned short versionMinor;		// Version minor number
      extern const char *version;			// Version identity
      extern const char *versionFull;			// Full version string
      extern const unsigned char libVersion;		// Library API version
      extern const unsigned char libRevision;		// Library API revision
      extern const unsigned char libAge;		// Library API age
      extern const char *bugsEmail;			// Bug report e-mail
      extern const char *versionInfo[];			// Information (/INFO)
      extern const char *versionChars;			// Version chars 
   };
};
   
#endif
   
