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

#ifndef _INCLUDE_KINEIRCD_MODULELIST_H_
# define _INCLUDE_KINEIRCD_MODULELIST_H_ 1

// Forwarded declarations (completed after class)
namespace Kine {
   class ModuleDescriptor;
};

# include <map>
# include <aisutil/string/string.h>

namespace Kine {
   class ModuleList {
    private:
      typedef std::multimap <const char* const, ModuleDescriptor* const>
	modulesMap_type;
      modulesMap_type modules;
      
    public:
      // Constructor
      ModuleList(void)
	{};
      
      // Destructor
      ~ModuleList(void)
	{ stopAll(); };
      
      // Load a module, return false if we failed (reason in 'errorReturn')
      ModuleDescriptor* const loadModule(const AISutil::String &moduleFile,
					 AISutil::String &errString);
      
      // Start all modules
      void startAll(void) const;
      
      // Stop and unload all modules
      void stopAll(void);
   };
};

// Complete forwarded declarations
# include <kineircd/moduledescriptor.h>

#endif // _INCLUDE_KINEIRCD_MODULELIST_H_
   
