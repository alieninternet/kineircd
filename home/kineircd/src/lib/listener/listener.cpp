/* $Id$
 * Listener descriptor class
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

#include "autoconf.h"
#include "kineircd/kineircdconf.h"

#include <sstream>
#include <aisutil/socket/sockets.h>

#include "kineircd/listener.h"
#include "debug.h"

using namespace Kine;


/* listen - Start listening on this listener socket
 * Original 11/08/2002 simonb
 */
const bool Listener::listen(void)
{
#ifdef KINE_DEBUG_PSYCHO
   std::ostringstream output;
   output << "Listener::listen() on file descriptor " << socket.getFD() <<
     " (" << socket.getLocalAddress() << " port " <<
     socket.getLocalPort() << ')';
   debug(output.str());
#endif
   
   // Make sure we are not already listening, and that the socket exists
   if (listening) {
#ifdef KINE_DEBUG_EXTENDED
      debug("Already listening on the socket");
#endif
      return false;
   }

   // Try and listen..
   if (!socket.listen(listenBacklog)) {
#ifdef KINE_DEBUG_EXTENDED
      debug("Could not listen on " + socket.getLocalAddress() + ": " +
	    socket.getErrorMessage());
#endif
      return false;
   }
   
   return (listening = true);
};
