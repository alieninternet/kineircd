/* $Id$
 * KineIRCd module interface for the IRC-2 user protocol module
 * 
 * Copyright (c) 2001,2002 KineIRCd Development Team
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

#include <kineircd/module.h>
#include <iostream>

#include "module.h"


namespace {
   /* moduleStart - Fire up the module
    * Original 05/10/2002 simonb
    */
   static KINE_MODULE_START(moduleStart) {
#ifdef KINE_DEBUG
      std::cerr << "Hello, my name is mod_irc2user ;)" << std::endl;
#endif
      
      // Be happy :)
      return true;
   }


   /* moduleStop - Shut down the module
    * Original 05/10/2002 simonb
    */
   static KINE_MODULE_STOP(moduleStop) {
      // something here one day..
   }
   

   // Basic information about this module
   static const Kine::Module::basicInfo_type moduleInfo = {
      // Firstly, we list the version/copyright information for this module
      MOD_IRC2USER_NAME_SHORT,
      MOD_IRC2USER_NAME_LONG,
      MOD_IRC2USER_COPYRIGHT,
      MOD_IRC2USER_VERSION_MAJOR, 
      MOD_IRC2USER_VERSION_MINOR,
      MOD_IRC2USER_VERSION_PATCHLEVEL,
      MOD_IRC2USER_VERSION_EXTRA,
      0,

      // Flags to define how we need to be run
      Kine::Module::basicInfo_type::Flags::UNIQUE_INSTANCE,

      // Configuration stuff
      0,
	
      // Our start and stop functions
      &moduleStart,
      &moduleStop
   };
}; // namespace {anonymous}


KINE_MODULE_INIT {
   return new Kine::Module(moduleInfo);
}
