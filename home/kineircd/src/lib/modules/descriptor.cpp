/* $Id$
 * A descriptor which describes modules.. what more did you expect? :)
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

extern "C" {
# include <dlfcn.h>
};

#include "kineircd/moduledescriptor.h"
#include "debug.h"

using namespace Kine;
using LibAIS::String;


/* ~ModuleDescriptor - Descriptor destructor, hehe
 * Original 20/07/2002 simonb
 */
ModuleDescriptor::~ModuleDescriptor(void)
{
   // Close the module if it appears to be still valid
   if (handle != 0) {
      (void)dlclose(handle);
   }
   
   // Delete the module
   delete module;
}
      

/* loadModule - Open a module using the given file
 * Original 04/07/2002 simonb
 */
ModuleDescriptor *ModuleDescriptor::loadModule(const String &moduleFile, 
					       String &errorReturn) 
{
#ifdef KINE_DEBUG_PSYCHO
   debug("ModuleDescriptor::loadModule() - Trying to load " + moduleFile);
#endif
   
   // Try and load the given module
   void *moduleHandle = dlopen(moduleFile.c_str(), RTLD_NOW);
   
   // Check if the module loaded okay
   if (moduleHandle == 0) {
      // Set the error string
      errorReturn = dlerror();
#ifdef KINE_DEBUG_PSYCHO
   debug("ModuleDescriptor::loadModule() - dlopen() gave: " + errorReturn);
#endif
      return 0;
   }

   char *error = 0;
   
   // Attempt to find the init function
   KINE_MODULE_INIT_PROTOTYPE((*initFunction)) =
     (KINE_MODULE_INIT_PROTOTYPE((*)))
     dlsym(moduleHandle, KINE_MODULE_INIT_SYMBOL_NAME);
   
   // Check if we found something
   if ((initFunction == 0) || 
       ((error = dlerror()) != 0)) {
      // Set the error string
      errorReturn = error;
      
      // Close the function
      (void)dlclose(moduleHandle);
      return 0;
   }
   
#ifdef KINE_DEBUG_PSYCHO
   debug("ModuleDescriptor::loadModule() - Running the init function in " +
	 moduleFile);
#endif

   // Initialise the module and grab its info!
   const Module *moduleInfo = (*initFunction)();

   // Make sure the module's init function did not return 0 (null)
   if (moduleInfo == 0) {
      errorReturn = "Module initialisation returned no information";
      return 0;
   }
   
   // Check the basic info structure
   if (!moduleInfo->getBasicInfo().isOkay()) {
      errorReturn = "Module contains an invalid basicInfo structure";
      return 0;
   }
   
   // Return a new module descriptor
   return new ModuleDescriptor(moduleHandle, moduleInfo);
}
