/* $Id$
 * 
 * Copyright (c) 2001,2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2002,2003 KineIRCd Development Team
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

#include "kineircd/myserver.h"
#include "kineircd/daemon.h"
#include "lib/registry.h"
#include "lib/debug.h"

using namespace Kine;


// Our instance...
MyServer* MyServer::instance = 0;


/* MyServer - Constructor
 * Original 20/03/2003 simonb
 */
MyServer::MyServer(void)
  : Server(Languages::toWideStr(config().getServerName()), daemon().getTime()),
    description(Languages::toWideStr(config().getServerDescription())) // bleh
{
   // Register ourself to the registry
#ifdef KINE_DEBUG_ASSERT
   assert(Internal::registry().addServer(*this) == Error::NO_ERROR);
#else
   (void)Internal::registry().addServer(*this);
#endif
}


/* initInstance - Create the single instance of this class
 * Original 20/03/2003 simonb
 */
void MyServer::initInstance(void)
{
#ifdef KINE_DEBUG_ASSERT
   // Make sure we are only called once
   assert(instance == 0);
#endif
   
   instance = new MyServer();

#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "MyServer::initInstance() - Created new instance @ " <<
     instance;
   debug(debugOut.str());
#endif
}


/* addUser - Add the given locally connected user
 * Original 13/09/2003
 */
const Error::error_type MyServer::addUser(LocalUser& user)
{
   // Fob this off to the addUser function
   return Server::addUser(user);
}
