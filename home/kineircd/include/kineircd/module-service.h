/* $Id$
 * Service modules (smod) - modules that add services/clients/channels etc
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

#ifndef _INCLUDE_KINEIRCD_MODULE_SERVICE_H_
# define _INCLUDE_KINEIRCD_MODULE_SERVICE_H_ 1

# include "kineircd/module.h"

namespace Kine {
   class ModuleService : public Module {
    public:
      // Constructor
      ModuleService(const Module::basicInfo_type &mi,
		    const ConfigData *cd = 0)
	: Module(Module::TYPE_SERVICE, mi, cd)
	{};
	  
      // Destructor
      ~ModuleService(void)
	{};
   };
};

#endif // _INCLUDE_KINEIRCD_MODULE_SERVICE_H_
   
