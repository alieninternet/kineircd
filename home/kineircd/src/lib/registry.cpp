/* $Id$
 * The registry, the place where all entities live
 * 
 * Copyright (c) 2001,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2003 KineIRCd Development Team
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

#include "kineircd/registry.h"
#include "libkineircd/debug.h"

using namespace Kine;


// Our instance...
Registry* Registry::instance = 0;


/* Registry - Initialise the registry
 * Original 09/04/2001 simonb
 */
Registry::Registry(void)
{
#ifdef KINE_DEBUG_ASSERT
   // Are we sane? Please say yes.. please say yes..
   assert(localUsers.empty());
#endif
}


/* initInstance - Create the single instance of this class
 * Original 07/04/2003 simonb
 */
void Registry::initInstance(void)
{
#ifdef KINE_DEBUG_ASSERT
   // We should only ever be called to init the first instance..
   assert(instance == 0);
#else
   
   // Well, since we are not debugging I suppose we should quietly ignore this
   if (instance != 0) {
# ifdef KINE_DEBUG
      debug("Registry::initInstance() - Called when an instance already "
	    "exists");
# endif
      return;
   }
#endif
   
   // Create the new instance
   instance = new Registry();
   
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Registry::initInstance() - Created new instance @ " << 
     instance;
   debug(debugOut.str());
#endif
}
