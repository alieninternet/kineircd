/* $Id$
 * Handles module loading/unloading and maintains the list of loaded modules
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

#include "kineircd/modulelist.h"
#include "lib/debug.h"

using namespace Kine;
using AISutil::String;


/* loadModule - Load a module, check it and add it to our list if it is okay
 * Original 21/07/2002 simonb
 */
ModuleDescriptor* const ModuleList::loadModule(const String& filename,
					 String& errString)
{
#ifdef KINE_DEBUG_PSYCHO
   debug("ModuleList::loadModule() - Trying to load " + filename);
#endif

   // Load the module
   ModuleDescriptor *descriptor =
     ModuleDescriptor::loadModule(filename, errString);
   
   // Make sure it loaded happily
   if (descriptor == 0) {
      return 0;
   }

#ifdef KINE_DEBUG_EXTENDED
   debug("ModuleList::loadModule() - Loaded module: " + filename);
#endif

   // Does this module only want to be loaded once?
   if ((descriptor->getModule().getInfo().flags &
	Module::Flags::UNIQUE_INSTANCE) &&
       (modules.count(descriptor->getModule().getKeyName()) > 1)) {
      errString = descriptor->getModule().getVersionString() + 
	" can only be loaded once";
      delete descriptor;
      return 0;
   }

   // Okay then, if we got this far we can add this module to the list
   (void)modules.insert(modulesMap_type::value_type
			(descriptor->getModule().getKeyName(), descriptor));
   
#ifdef KINE_DEBUG_PSYCHO
   debug("ModuleList::loadModule() - Added " +
	 descriptor->getModule().getVersionString() + 
	 " to the module list (" + String::convert(modules.size()) +
	 " modules currently loaded)");
   
   // If the module has the versionInfo variable, output that stuff too :)
   if (descriptor->getModule().getInfo().versionInfo != 0) {
      debug("ModuleList::loadModule() - Module has versionInfo list:");
      for (int i = 0;
	   descriptor->getModule().getInfo().versionInfo[i] != 0;
	   i++) {
	 debug(String("     -=> ") + 
	       descriptor->getModule().getInfo().versionInfo[i]);
      }
   }
#endif
   
   // Smile, it all worked out okay :)
   return descriptor;
}


/* startAll - Start all modules
 * Original 22/07/2002 simonb
 */
void ModuleList::startAll(void) const
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
	    (*it).second->getModule().getVersionString());
#endif
      (*it).second->start();
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
	    (*it).second->getModule().getVersionString());
#endif
      delete (*it).second;
   }
   
   // Wipe out the now defunct modules list
   modules.clear();
}
