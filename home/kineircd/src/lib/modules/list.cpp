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

#include "config.h"

#include "kineircd/modulelist.h"
#include "debug.h"

using namespace Kine;
using AISutil::String;


/* loadModule - Load a module, check it and add it to our list if it is okay
 * Original 21/07/2002 simonb
 */
ModuleDescriptor *ModuleList::loadModule(const String &moduleFile,
					 String &errorReturn)
{
#ifdef KINE_DEBUG_PSYCHO
   debug("ModuleList::loadModule() - Trying to load " + moduleFile);
#endif

   // Load the module
   ModuleDescriptor *moduleDesc =
     ModuleDescriptor::loadModule(moduleFile, errorReturn);
   
   // Make sure it loaded happily
   if (moduleDesc == 0) {
      return 0;
   }

#ifdef KINE_DEBUG_EXTENDED
   debug("ModuleList::loadModule() - Loaded module: " + moduleFile);
#endif

   // Does this module only want to be loaded once?
   if ((moduleDesc->getModule()->getBasicInfo().flags & 
	Module::basicInfo_type::FLAG_UNIQUE_INSTANCE) &&
       (modules.count(moduleDesc->getModule()->getKeyName()) > 1)) {
      errorReturn = moduleDesc->getModule()->getVersionString() +
	" can only be loaded once";
      delete moduleDesc;
      return 0;
   }

   // Okay then, if we got this far we can add this module to the list
   (void)modules.insert(modulesMap_type::value_type
			(moduleDesc->getModule()->getKeyName(), moduleDesc));
   
#ifdef KINE_DEBUG_PSYCHO
   debug("ModuleList::loadModule() - Added " + 
	 moduleDesc->getModule()->getVersionString() + 
	 " to the module list (" + String::convert(modules.size()) +
	 " modules currently loaded)");
#endif
   
   // Smile, it all worked out okay
   return moduleDesc;
}


/* startAll - Start all modules
 * Original 22/07/2002 simonb
 */
void ModuleList::startAll(Daemon &daemon) const
{
#ifdef KINE_DEBUG_EXTENDED
   debug("ModuleList::startAll() - Starting " + 
	 String::convert(modules.size()) + " modules");
#endif
   
   // Run through the list of modules and call their start functions
   for (modulesMap_type::const_iterator it = modules.begin(); 
	it != modules.end(); it++) {
#ifdef KINE_DEBUG_PSYCHO
      debug("ModuleList::startAll() - Starting: " +
	    (*it).second->getModule()->getVersionString());
#endif
      (*it).second->start(daemon);
   }
}


/* stopAll - Stop and unload all modules
 * Original 22/07/2002 simonb
 */
void ModuleList::stopAll(void)
{
#ifdef KINE_DEBUG_EXTENDED
   debug("ModuleList::stopAll() - Stopping " + 
	 String::convert(modules.size()) + " modules");
#endif
   
   // Run through the list of modules to stop and delete them all
   for (modulesMap_type::const_iterator it = modules.begin(); 
	it != modules.end(); it++) {
#ifdef KINE_DEBUG_PSYCHO
      debug("ModuleList::stopAll() - Stopping: " +
	    (*it).second->getModule()->getVersionString());
#endif
      (*it).second->stop();
      delete (*it).second;
   }
   
   // Wipe out the now defunct modules list
   modules.clear();
}
