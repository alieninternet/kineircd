/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
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

#include "kineircd/mynetwork.h"
#include "kineircd/config.h"
#include "kineircd/daemon.h"
#include "lib/debug.h"

using namespace Kine;


// Our instance...
MyNetwork* MyNetwork::instance = 0;


/* MyNetwork - Constructor
 * Original 21/10/2003 simonb
 */
MyNetwork::MyNetwork(void)
  : Network(Languages::toWideStr(config().getNetworkName()),
	    daemon().getTime())
{
   // Register ourself to the registry?
}


/* initInstance - Create the single instance of this class
 * Original 21/10/2003 simonb
 */
void MyNetwork::initInstance(void)
{
#ifdef KINE_DEBUG_ASSERT
   // Make sure we are only called once
   assert(instance == 0);
#endif
   
   instance = new MyNetwork();

#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "MyNetwork::initInstance() - Created new instance @ " <<
     instance;
   debug(debugOut.str());
#endif
}
