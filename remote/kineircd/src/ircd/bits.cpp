/* $Id$
 * 
 * Copyright (c) 2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002,2003 KineIRCd Development Team
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

#include <iostream>
#include <aisutil/version.h>
#include <kineircd/version.h>

extern "C" {
#ifdef KINE_HAVE_OPENSSL
# include <openssl/opensslv.h>
#endif
#include <options.h>
}

#include "ircd/bits.h"

using namespace Kine;


/* printVersion - Print the 'version' of the program (copyright and more etc)
 * Original 30/03/2002 simonb
 */
void ircd::printVersion(const unsigned int level)
{
   // This bit always gets out.. it's important ;)
   std::cout << Version::versionFull << '\n';

   // More detail needed?
   if (level > 1) {
      std::cout << "\n"
	"  Core build time:  " << Version::buildTimeFull << " (Build #" <<
	Version::buildCount << ")\n"
	"     Build traits:  " << Version::versionChars << "\n"
	" Core source hash:  " << Version::sourceCodeHash << "\n"
	"\n"
	"Linked to the following libraries:\n"
	"   KineIRCd Library API version " <<
	(unsigned int)Version::libVersion << '.' << 
	(unsigned int)Version::libRevision << '.' <<
	(unsigned int)Version::libAge << "\n"
	"   LibAISutil Library version " << 
	AISutil::Version::version << " (API version " <<
	(unsigned int)AISutil::Version::libVersion << '.' << 
	(unsigned int)AISutil::Version::libRevision << '.' <<
	(unsigned int)AISutil::Version::libAge << ")\n"
	"   AutoGen AutoOpts (libopts) version " <<
	OPTIONS_VERSION_STRING << "\n"
#ifdef KINE_HAVE_OPENSSL
	"   " << OPENSSL_VERSION_TEXT << "\n"
#endif
	/* terminate the line cout line */ ;
      
      // Even more detail - pump out the core's hard-coded INFO command data
      if (level > 2) {
	 std::cout << 
	   "\nCore /INFO command data (may contain UTF-8 characters):\n";
	 
	 for (int i = 0; Version::versionInfo[i] != 0; i++) {
	    std::cout << Version::versionInfo[i] << '\n';
	 }
      }
   }
   
   // Flush the output
   std::cout << std::flush;
}
