/* $Id$
 * Protocol modules (pmod) - modules that add protocols beyond registration
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

#ifndef _INCLUDE_KINEIRCD_MODULE_PROTOCOL_H_
# define _INCLUDE_KINEIRCD_MODULE_PROTOCOL_H_ 1

# include "kineircd/module.h"

namespace Kine {
   class ModuleProtocol : public Module {
    public:
      // Constructor
      ModuleProtocol(const Module::basicInfo_type& mi,
		     const ConfigData* const cd = 0)
	: Module(Module::TYPE_PROTOCOL, mi, cd)
	{};
	  
      // Destructor
      ~ModuleProtocol(void)
	{};
   };
};

#endif // _INCLUDE_KINEIRCD_MODULE_PROTOCOL_H_
   
