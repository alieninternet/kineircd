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

   class Version {
    private:
      // This cannot be constructed.
      Version(void) {};
      
    public:
      static char const *build;			// Build information
      static char const *programName;		// Program name (simple)
      static unsigned short const versionMajor;	// Program version major number
      static unsigned short const versionMinor;	// Program version minor number
      static char const *version;		// Version identity
      static char const *versionFull;		// Full version identity
      static char const *bugsEmail;		// Bug report e-mail address
      static char const *versionInfo[];		// Information (for INFO)
      static char const *versionChars;		// Version chars (for VERSION)
   };
   
};
   
#endif
   
