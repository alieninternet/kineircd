/* $Id$
 * Handles listener status of the listeners list over-all
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

#include "kineircd/listenerlist.h"
#include "socket/sockets.h"
#include "debug.h"

using namespace Kine;


/* startAll - Start all listeners listening
 * Original 07/08/2002 simonb
 */
void ListenerList::startAll(void)
{
#ifdef KINE_DEBUG_EXTENDED
   debug("ListenerList::startAll() - Starting " + 
	 String::convert(listeners.size()) + " listening sockets");
#endif
   
   // Run through the list of modules and call their start functions
   for (listeners_type::iterator it = listeners.begin(); 
	it != listeners.end(); it++) {
      if (!(*it).listen()) {
#ifdef KINE_DEBUG
	 debug("ListenerList::startAll() - oops");
#endif
      }
   }
}


   // "LISTEN" class
const ConfigParser::defTable_type ListenerList::configClassDefs = {
     {
	"ALLOWSERVERS",
	  0,
	  0,
	  0,
	  0
     },
     {
	"ALLOWSERVICES",
	  0,
	  0,
	  0,
	  0
     },
     {
	"ALLOWUSERS",
	  0,
	  0,
	  0,
	  0
     },
     {
	"PORT",
	  0,
	  0,
	  0,
	  0
     },
     {
	"SECURE",
	  0,
	  0,
	  0,
	  0
     },
     {
	0,
	  0,
	  0,
	  0,
	  0
     }
};


/* classHandleListen
 * Original 11/08/2002 simonb
 */
CONFIG_CLASS_HANDLER(ListenerList::configClassHandler)
{
   // Check if the first value is empty (the listen type field)
   if (values.empty() || values.front().empty()) {
      // Get cranky
      errString = "You must specify the domain/address type for listening";
      return false;
   }

   // Upper-case the domain for easy checking
   String domain = values.front().toUpper();

   // The socket we will be listening on, it cannot be null
   Socket *socket = 0;
   
   // Try and determine the domain
#ifdef KINE_HAVE_SOCKET_IPV4_TCP
   if ((domain == "IPV4") || (domain == "TCP/IPV4")) {
# ifdef KINE_DEBUG_PSYCHO
      debug("ListenerList::configClassHandler() - TCP/IPv4 domain chosen");
# endif
      socket = new SocketIPv4TCP();
   } else 
#endif
#ifdef KINE_HAVE_SOCKET_IPV6_TCP
   if ((domain == "IPV6") || (domain == "TCP/IPV6")) {
# ifdef KINE_DEBUG_PSYCHO
      debug("ListenerList::configClassHandler() - TCP/IPv6 domain chosen");
# endif
      socket = new SocketIPv6TCP();
   } else
#endif
#ifdef KINE_HAVE_SOCKET_IPX_SPX
   if ((domain == "IPX") || (domain == "SPX") || (domain == "IPX/SPX")) {
# ifdef KINE_DEBUG_PSYCHO
      debug("ListenerList::configClassHandler() - IPX/SPX domain chosen");
# endif
      socket = new SocketIPXSPX();
   } else
#endif
#ifdef KINE_HAVE_SOCKET_UNIX
   if (domain == "UNIX") {
# ifdef KINE_DEBUG_PSYCHO
      debug("ListenerList::configClassHandler() - UNIX domain chosen");
# endif
      socket = new SocketUNIX();
   } else
#endif
   {
      // No idea what they want, complain
      errString = "Unknown or unsupported domain/address type: '" + 
	values.front() + '\'';
      return false;
   }

#ifdef KINE_DEBUG_ASSERT
   // Make sure we are not going insane
   assert(socket != 0);
#endif
   
   // uhh something here, simon.
   
   // Throw the listener stuff over to the configuration parser..
   return ConfigParser::parse(configData, position, (void *)(configClassDefs),
			      dataClass); // <=- that is temporary :(
}


