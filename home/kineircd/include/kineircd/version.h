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
 */

#ifndef _INCLUDE_KINEIRCD_VERSION_H_
# define _INCLUDE_KINEIRCD_VERSION_H_ 1

namespace Kine {
   //! Version and Compile information
   namespace Version {
      //! The time the core was built in compressed ISO-8601, from UTC
      extern const char* const buildTime;
      
      //! The time the core was built, as per your locale (in your timezone)
      extern const char* const buildTimeFull;

      //! The number of times we were built (or rebuilt)
      extern const unsigned long buildCount;
      
      //! Source code 'hash' - a string unique to the library's source code
      extern const char* const sourceCodeHash;
      
      //! The name of the program (in short-form)
      extern const char* const programName;
      
      //! The full name of the program
      extern const char* const programNameFull;
      
      //! The major version number
      extern const unsigned short versionMajor;
      
      //! The minor version number
      extern const unsigned short versionMinor;
      
      //! The version of the program (the short name plus major/minor numbers)
      extern const char* const version;
      
      //! The full version string (same as 'version', but includes build time)
      extern const char* const versionFull;

      //! The core's API version number
      extern const unsigned char libVersion;
      
      //! The core's API revision number
      extern const unsigned char libRevision;

      //! The core's API age number
      extern const unsigned char libAge;
      
      //! Contact address to send bugs, feature requests, and so forth
      extern const char* const bugsEmail;
      
      //! Built in version information used by the 'INFO' user command
      extern const char* const versionInfo[];
      
      //! Version 'chars', used to show which compile-time flags were set
      extern const char* const versionChars;
   };
};
   
#endif // _INCLUDE_KINEIRCD_VERSION_H_
   
