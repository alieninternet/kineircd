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

#include "modules/descriptor.h"

using namespace Kine;


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
      
