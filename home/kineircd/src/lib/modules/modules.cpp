/* $Id$
 * Support class for modules
 * 
 * Copyright (c) 2002 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002 KineIRCd Development Team
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include <sstream>

#include "kineircd/module.h"

using namespace Kine;
using AISutil::String;


/* getVersionString - Return the module name in full version format
 * Original 22/07/2002 simonb
 */
const String Module::getVersionString(void) const
{
   std::ostringstream output;
   
   // Add the start of the string, these are required fields
   output << getInfo().nameShort << '-' << 
     (unsigned int)getInfo().versionMajor << '.' << 
     (unsigned int)getInfo().versionMinor;
   
   // Do we need to add the patch level?
   if (getInfo().versionPatchLevel > 0) {
      output << '.' << getInfo().versionPatchLevel;
   }
   
   // Do we need to add the extra version info?
   if (getInfo().versionExtra != 0) {
      output << getInfo().versionExtra;
   }
   
   return output.str();
};
