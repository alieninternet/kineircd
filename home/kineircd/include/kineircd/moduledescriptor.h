/* $Id$
 * A descriptor which describes modules.. what more did you expect? :)
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

#ifndef _INCLUDE_KINEIRCD_MODULEDESCRIPTOR_H_
# define _INCLUDE_KINEIRCD_MODULEDESCRIPTOR_H_ 1

# include <aisutil/string/string.h>

// This open dependance on ltdl publicly is disturbing. Hopefully it will
// disappear if this file is removed from public view..
extern "C" {
# include <ltdl.h>
}

# include <kineircd/module.h>

namespace Kine {
   class ModuleDescriptor {
    private:
      lt_dlhandle handle;			// Module handle from dlopen()
      Module& module;				// The module itself
      
      // Constructor
      ModuleDescriptor(lt_dlhandle& h, Module& m)
	: handle(h),
          module(m)
	{};
      
    public:
      // Destructor
      ~ModuleDescriptor(void);
      
      // Return the module
      Module& getModule(void)
	{ return module; };

      // Load a module, hopefully. Returns 0 (null) if we failed
      static ModuleDescriptor* const
	loadModule(const AISutil::String& moduleFile,
		   AISutil::String& errString);
      
      // Start a module
      bool start(void)
	{ return module.start(); }
   };
};

#endif // _INCLUDE_KINEIRCD_MODULEDESCRIPTOR_H_
   
