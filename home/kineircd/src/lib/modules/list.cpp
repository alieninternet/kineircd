/* $Id$
 * Handles module loading/unloading and maintains the list of loaded modules
 * 
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

#include "kineircd/kineircdconf.h"

#include "kineircd/modulelist.h"
#include "debug.h"

using namespace Kine;


/* loadModule - Load a module, check it and add it to our list if it is okay
 * Original 21/07/2002 simonb
 */
bool ModuleList::loadModule(const String &moduleFile, String &errorReturn)
{
#ifdef KINE_DEBUG_PSYCHO
   debug("ModuleList::loadModule() - Trying to load " + moduleFile);
#endif
   
   ModuleDescriptor *desc =
     ModuleDescriptor::loadModule(moduleFile, errorReturn);
   
   // Make sure it loaded happily
   if (desc == 0) {
      return false;
   }

#ifdef KINE_DEBUG_EXTENDED
   debug("ModuleList::loadModule() - Loaded module: " + moduleFile);
#endif
   
   // Smile, it all worked out okay
   return true;
}
