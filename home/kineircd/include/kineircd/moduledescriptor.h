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

#ifndef _SRC_LIB_MODULES_DESCRIPTOR_H_
# define _SRC_LIB_MODULES_DESCRIPTOR_H_ 1

# include "kineircd/module.h"
# include "kineircd/str.h"

namespace Kine {
   class ModuleDescriptor {
    private:
      void *handle;				// Module handle from dlopen()
      const Module *module;			// The module's info itself
      
      // Constructor
      ModuleDescriptor(void *h, const Module *m)
	: handle(h),
          module(m)
	{};
      
    public:
      // Destructor
      ~ModuleDescriptor(void);
      
      // Return the module
      const Module *getModule(void) const
	{ return module; };

      // Load a module, hopefully. Returns 0 (null) if we failed
      static ModuleDescriptor *openModule(const String &moduleFile,
					  String &errorReturn);
   };
};
   
#endif // _SRC_LIB_MODULES_DESCRIPTOR_H_
   
