/* vars.cpp
 * Configuration variable handlers
 * 
 * Copyright (c) 2002 AustHex Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of AustHex.
 * 
 * AustHex is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AustHex is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with AustHex; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "autoconf.h"

#include "config.h"
#include "configdefault.h"
#include "log.h"


// Our constructor
Config::Config(const String &f)
  : parser(&Config::topDefs, f),
    defAdminEmail(DEFAULT_ADMIN_EMAIL),
    defAdminLocation(DEFAULT_ADMIN_LOCATION),
    defAdminName(DEFAULT_ADMIN_NAME),
    defLoggingLog(0)
{
   defLoggingLog = new Log();  // temporary
};


// This is temporary.
CONFIG_DEF_HANDLER(Config::defVarNONE)
{
   return true;
}
