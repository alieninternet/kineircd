/* $Id$
 * Listener configuration parser helper class
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

#include <cstdlib>

#include "listener/config.h"
#include "socket/sockets.h"
#include "debug.h"

using namespace Kine;


// "LISTEN" class
const ConfigParser::defTable_type ListenerConfig::classDefs = {
     {
	"ADDRESS",
	  (void *)&ListenerConfig::varAddress,
	  &varHandleString,
	  0,
	  0
     },
     {
	"ALLOWNETWORKS",
	  (void *)&ListenerConfig::varAllowNetworks,
	  &varHandleBoolean,
	  0,
	  0
     },
     {
	"ALLOWSERVERS",
	  (void *)&ListenerConfig::varAllowServers,
	  &varHandleBoolean,
	  0,
	  0
     },
     {
	"ALLOWSERVICES",
	  (void *)&ListenerConfig::varAllowServices,
	  &varHandleBoolean,
	  0,
	  0
     },
     {
	"ALLOWUSERS",
	  (void *)&ListenerConfig::varAllowUsers,
	  &varHandleBoolean,
	  0,
	  0
     },
     {
	"PORT",
	  (void *)&ListenerConfig::varPort,
	  &varHandleString,
	  0,
	  0
     },
     {
	"SECURE",
	  (void *)&ListenerConfig::varSecure,
	  &varHandleBoolean,
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


/* ListenerConfig - Constructor (sets up default values)
 * Original 14/08/2002 simonb
 */
ListenerConfig::ListenerConfig(void)
  : varAllowNetworks(false),
    varAllowServers(false),
    varAllowServices(false),
    varAllowUsers(false),
    varSecure(false)
{
}


/* classHandleListen
 * Original 18/08/2002 simonb
 */
bool ListenerConfig::setupSocket(Socket& socket, String& errString, int port)
{
   // Set its address
   if (!varAddress.empty()) {
      if (!socket.setLocalAddress(varAddress)) {
	 // Delete the socket and complain
	 delete &socket;
	 errString = "Invalid socket address '" + varAddress + '\'';
	 return false;
      }
   }

   // Set up the port, if it isn't 0
   if (port != 0) {
      if (!socket.setLocalPort(port)) {
	 // Delete the socket and complain
	 delete &socket;
	 errString = "Invalid port number '" + varPort + '\'';
	 return false;
      }
   }
      
   // Bind
   if (!socket.bind()) {
      // Delete the socket and complain
      delete &socket;
      errString = "Unable to bind on '" + varAddress + '\'';
      return false;
   }
   
   // All worked out okay
   return true;
}

/* classHandleListen
 * Original 11/08/2002 simonb
 */
CONFIG_CLASS_HANDLER(ListenerConfig::classHandler)
{
   // Create our temporary config data class for the listener
   ListenerConfig config;
   
      // Throw the listener stuff over to the configuration parser..
   if (!ConfigParser::parse(configData, position, (void *)(classDefs),
			    config)) {
      return false;
   }

   // Work out the flags..
   Listener::flags_type flags = 0;
   if (config.varAllowNetworks) {
      flags &= Listener::FLAG_ALLOW_NETWORKS;
   }
   if (config.varAllowServers) {
      flags &= Listener::FLAG_ALLOW_SERVERS;
   }
   if (config.varAllowServices) {
      flags &= Listener::FLAG_ALLOW_SERVICES;
   }
   if (config.varAllowUsers) {
      flags &= Listener::FLAG_ALLOW_USERS;
   }
   
   /* Since we may be creating multiple sockets here, we need to know what
    * type of socket we are dealing with before we create it/them. We also
    * need to know what type of socket we are dealing with as far as how to
    * determine port numbers. If the port is in name form (i.e. a string) we
    * will need to look it up somehow, which depends on the type of socket.
    * Some socket types don't even have ports! This enumeration helps us out..
    */
   enum {
      DOMAIN_UNKNOWN,
#ifdef KINE_HAVE_SOCKET_IPV4_TCP
      DOMAIN_IPV4,
#endif
#ifdef KINE_HAVE_SOCKET_IPV6_TCP
      DOMAIN_IPV6,
#endif
#ifdef KINE_HAVE_SOCKET_IPX_SPX
      DOMAIN_IPX,
#endif
#ifdef KINE_HAVE_SOCKET_UNIX
      DOMAIN_UNIX
#endif
   } domainType = DOMAIN_UNKNOWN;
   
   // Check if the first value is empty (the listen type field)
   if (!values.empty() && !values.front().empty()) {
      // Upper-case the domain for easy checking
      String domain = values.front().toUpper();
      
      // Try and determine the domain
#ifdef KINE_HAVE_SOCKET_IPV4_TCP
      if ((domain == "IPV4") || (domain == "TCP/IPV4")) {
# ifdef KINE_DEBUG_PSYCHO
	 debug("ListenerConfig::classHandler() - TCP/IPv4 socket chosen");
# endif
	 domainType = DOMAIN_IPV4;
      } else 
#endif
#ifdef KINE_HAVE_SOCKET_IPV6_TCP
      if ((domain == "IPV6") || (domain == "TCP/IPV6")) {
# ifdef KINE_DEBUG_PSYCHO
	 debug("ListenerConfig::classHandler() - TCP/IPv6 socket chosen");
# endif
	 domainType = DOMAIN_IPV6;
      } else
#endif
#ifdef KINE_HAVE_SOCKET_IPX_SPX
      if ((domain == "IPX") || (domain == "SPX") || (domain == "IPX/SPX")) {
# ifdef KINE_DEBUG_PSYCHO
	 debug("ListenerConfig::classHandler() - IPX/SPX socket chosen");
# endif
	 domainType = DOMAIN_IPX;
      } else
#endif
#ifdef KINE_HAVE_SOCKET_UNIX
      if (domain == "UNIX") {
# ifdef KINE_DEBUG_PSYCHO
	 debug("ListenerConfig::classHandler() - UNIX socket chosen");
# endif
	 domainType = DOMAIN_UNIX;
      } else
#endif
      {
	 // No idea what they want, complain
	 errString = "Unknown or unsupported domain/address type: '" + 
	   values.front() + '\'';
	 return false;
      }
   }

   // If we still do not have the socket created by now, try to make a guess..
   if (domainType == DOMAIN_UNKNOWN) {
#ifdef KINE_HAVE_SOCKET_IPV4_TCP
      if (false) {
# ifdef KINE_DEBUG_PSYCHO
	 debug("ListenerConfig::classHandler() - "
	       "TCP/IPv4 socket determined from address");
# endif
	 domainType = DOMAIN_IPV4;
      } else
#endif
#ifdef KINE_HAVE_SOCKET_IPV6_TCP
      if (false) {
# ifdef KINE_DEBUG_PSYCHO
	 debug("ListenerConfig::classHandler() - "
	       "TCP/IPv6 socket determined from address");
# endif
	 domainType = DOMAIN_IPV6;
      } else
#endif
      {
	 // Cannot work out what type of socket is required, so we will give up
	 errString = 
	   "Unable to determine the domain/address type, please specify which "
	   "address type the listener is for.";
	 return false;
      }
   }
   
#ifdef KINE_DEBUG_ASSERT
   // Make sure we are not going insane - we must have the domain sorted by now
   assert(domainType != DOMAIN_UNKNOWN);
#endif
   
#ifdef KINE_HAVE_SOCKET_UNIX
   // If the socket type is unix - in that case we don't care about ports.
   if (domainType == DOMAIN_UNIX) {
      // Make sure the address exists!
      if (config.varAddress.empty()) {
	 // Complain
	 errString = "UNIX sockets require a filename for an address";
	 return false;
      }
      
# ifdef KINE_DEBUG_PSYCHO
      debug("ListenerConfig::classHandler() - Creating a UNIX socket...");
# endif
      // Create the socket and set it up
      SocketUNIX *socket = new SocketUNIX();
      if (!config.setupSocket(*socket, errString)) {
	 return false;
      }
      
      // Add it to the list
      (dataClass.*((ListenerList ConfigData::*)dataVariable)).listeners.
	push_front(Listener(*socket, flags));
      return true;
   }
#endif
   
   // Try and convert the port number over (note the base variable on strtol())
   char *portEndPtr = 0;
   int port = strtol(config.varPort.c_str(), &portEndPtr, 0);

   // Check if that worked
   if (*portEndPtr != 0) {
      errString = "bah.";
cout << "************************** IS TEXT" << endl;
      return false;
   }
   
   // Check the port is greater than 0
   if (port <= 0) {
      errString = "Invalid port number '" + config.varPort + '\'';
//      return false;
return true; // temporary.
   }

#ifdef KINE_DEBUG_PSYCHO
   debug("Port discovered was " + String::convert(port));
#endif
   
   /* Okay, well the port number was valid, and being a single number we
    * will only need to create ONE instance of this listener, rather than
    * look up the port(s) and potentially create multiple instances.
    */
   Socket *socket = 0;
#ifdef KINE_HAVE_SOCKET_IPV4_TCP
   if (domainType == DOMAIN_IPV4) {
      socket = new SocketIPv4TCP();
   } else
#endif   
#ifdef KINE_HAVE_SOCKET_IPV6_TCP
   if (domainType == DOMAIN_IPV6) {
      socket = new SocketIPv6TCP();
   } else
#endif
#ifdef KINE_HAVE_SOCKET_IPX_SPX
   if (domainType == DOMAIN_IPX) {
      socket = new SocketIPXSPX();
   } else
#endif
   { /* explode? */ };
   
#ifdef KINE_DEBUG_ASSERT
   // Make sure we are not insane - we must have gotten a socket here.
   assert(socket != 0);
#endif
   
   if (!config.setupSocket(*socket, errString, port)) {
      return false;
   }
   
   // Add it to the list
   (dataClass.*((ListenerList ConfigData::*)dataVariable)).listeners.
     push_front(Listener(*socket, flags));
   
   // All is well
   return true;
}
