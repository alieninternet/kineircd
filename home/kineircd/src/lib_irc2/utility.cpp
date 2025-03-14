/* $Id$
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

#include <kineircd/mynetwork.h>

#include "kineircd/libirc2/utility.h"

using namespace Kine::LibIRC2;


/* findMessageTarget - Find a message's destination by the given identity
 * Original 01/09/2001 simonb
 * Note:
 *   This gets complicated, since there are many forms:
 *    nickname
 *    nickname@server
 *    nickname%hostname
 *    nickname%hostname@server
 *    nickname!username@hostname            <=- odd one out, the @ = hostname
 *    nickname!username%hostname@server
 *    nickname#network
 *    nickname@server#network
 *    nickname%hostname#network
 *    nickname%hostname@server#network
 *    nickname!username@hostname#network    <=- also odd one out..
 *    nickname!username%hostname@server#network
 * 
 *   Channels:
 *    <char>name
 * 
 *   But this does not include:
 *    #*.tld (user host mask)
 *    $*.tld (server host mask)
 * 
 *   .. Try to stay with me :)
 */
Kine::Receiver* const
  Utility::findMessageTarget(const std::wstring& target,
			     Kine::Receiver::Directivity& directivity,
			     const bool includeServices)
{
   // Sanity!
   if (!target.empty()) {
      // Okay, check if this is a channel!
      if (/* ?? */ false) {
	 // Find and return the channel, if we can..
	 return myNetwork().findChannel(target);
      }
      
      /* Okay, then presume this is a nickname.. This is the bit
       * where it gets complicated :)
       */
      Kine::Receiver* receiver;
      
      // Find a client with just the name
      receiver = myNetwork().findUser(target);

      // After all that, did we find a user?
      if (receiver != 0) {
	 return receiver;
      }
      
      /* If we did not find anything, and we are supposed to include services
       * (although it is contrary to the RFC), try the same search, but on the
       * services list
       */
      if (includeServices) {
	 // Try to find the service
	 receiver = myNetwork().findService(target);
	 
	 // Did we find it?
	 if (receiver != 0) {
	    return receiver;
	 }
      }
   }
   
   // No idea, return a null pointer
   return 0;
}


/* findServerTarget - Find a server as a target from the given mask
 * Original 29/11/2003 simonb
 */
Kine::Server* const Utility::findServerTarget(const std::wstring& mask)
{
   // Is the given mask actually a mask?
   if (mask.find_first_of(L"*?") == std::wstring::npos) {
      // It's not a mask, so first of all, check the server list
      Server* const serverPtr = myNetwork().findServer(mask);
      if (serverPtr != 0) {
	 // Found it, return it!
	 return serverPtr;
      }
      
      // Okay, look for a client name instead then..
      Client* const clientPtr = myNetwork().findClient(mask);
      if (clientPtr != 0) {
	 // Found it, return server the client is connected to
	 return &(clientPtr->getServer());
      }
   } else {
      // It's a mask, go over the server list
#ifdef KINE_DEBUG
# warning "Cannot check server list with masks until query facility is written"
#endif
   }
   
   // Hrrm, out of ideas - return a null pointer
   return 0;
}
