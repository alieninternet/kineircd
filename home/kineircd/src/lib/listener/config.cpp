/* $Id$
 * 
 * Copyright (c) 2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002,2003 KineIRCd Development Team
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

#include <cstdlib>
#include <cstring>
#include <cerrno> // temporary
#ifdef HAVE_NETDB_H
# include <netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#include <aisutil/socket/sockets.h>

#include "libkineircd/listener/config.h"
#include "libkineircd/debug.h"

using namespace Kine;
using AISutil::String;


// Protocol names used when getting information via getservent()
const char* ListenerConfig::tcpProtocolName = "TCP";


// "LISTEN" class
const AISutil::ConfigParser::defTable_type ListenerConfig::classDefs = {
     {
	"ADDRESS", 4,
	  (void *)&ListenerConfig::varAddress,
	  &varHandleString,
	  0,
	  0
     },
     {
	"ALLOWNETWORKS", 8,
	  (void *)&ListenerConfig::varAllowNetworks,
	  &varHandleBoolean,
	  0,
	  0
     },
     {
	"ALLOWSERVERS", 11,
	  (void *)&ListenerConfig::varAllowServers,
	  &varHandleBoolean,
	  0,
	  0
     },
     {
	"ALLOWSERVICES", 12,
	  (void *)&ListenerConfig::varAllowServices,
	  &varHandleBoolean,
	  0,
	  0
     },
     {
	"ALLOWUSERS", 9,
	  (void *)&ListenerConfig::varAllowUsers,
	  &varHandleBoolean,
	  0,
	  0
     },
     {
	"LISTENBACKLOG", 13,
	  (void *)&ListenerConfig::varListenBacklog,
	  &varHandleUnsignedShortNoZero,
	  0,
	  0
     },
     {
	"PORT", 4,
	  (void *)&ListenerConfig::varPort,
	  &varHandleString,
	  0,
	  0
     },
     {
	"SECURE", 3,
	  (void *)&ListenerConfig::varSecure,
	  &varHandleBoolean,
	  0,
	  0
     },
     {
	0, 0,
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
    varListenBacklog(LIBAISUTIL_SOCKET_DEFAULT_LISTEN_BACKLOG),
    varSecure(false)
{
}


/* classHandleListen
 * Original 18/08/2002 simonb
 */
bool ListenerConfig::setupSocket(AISutil::Socket& socket, 
				 AISutil::String& errString, int port)
{
   // Set its address
   if (!varAddress.empty()) {
      if (!socket.setLocalAddress(varAddress)) {
	 // Delete the socket and complain
	 errString = "Invalid socket address '" + varAddress + "': " +
	   socket.getErrorMessage();
	 delete &socket;
	 return false;
      }
   }

   // Set up the port, if it isn't 0
   if (port != 0) {
      if (!socket.setLocalPort(port)) {
	 // Delete the socket and complain
	 errString = "Invalid port number '" + varPort + "': " +
	   socket.getErrorMessage();
	 delete &socket;
	 return false;
      }
   }

   // Bind
   if (!socket.bind()) {
      // Delete the socket and complain
      errString = "Unable to bind on '" + socket.getLocalAddress() + "': " +
	socket.getErrorMessage();
      delete &socket;
      return false;
   }
   
   // All worked out okay
   return true;
}

/* classHandleListen
 * Original 11/08/2002 simonb
 */
LIBAISUTIL_CONFIG_CLASS_HANDLER(ListenerConfig::classHandler)
{
   // Create our temporary config data class for the listener
   ListenerConfig config;
   
      // Throw the listener stuff over to the configuration parser..
   if (!AISutil::ConfigParser::parse(configData, position, 
				     (void *)(classDefs), config)) {
      return false;
   }

   // Work out the flags..
   Listener::flags_type flags = 0;
   if (config.varAllowNetworks) {
      flags |= Listener::Flags::ALLOW_NETWORKS;
   }
   if (config.varAllowServers) {
      flags |= Listener::Flags::ALLOW_SERVERS;
   }
   if (config.varAllowServices) {
      flags |= Listener::Flags::ALLOW_SERVICES; 
  }
   if (config.varAllowUsers) {
      flags |= Listener::Flags::ALLOW_USERS;
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
#ifdef LIBAISUTIL_HAVE_SOCKET_IPV4_TCP
      DOMAIN_IPV4,
#endif
#ifdef LIBAISUTIL_HAVE_SOCKET_IPV6_TCP
      DOMAIN_IPV6,
#endif
#ifdef LIBAISUTIL_HAVE_SOCKET_IPX_SPX
      DOMAIN_IPX,
#endif
#ifdef LIBAISUTIL_HAVE_SOCKET_UNIX
      DOMAIN_UNIX
#endif
   } domainType = DOMAIN_UNKNOWN;
   
   // Check if the first value is empty (the listen type field)
   if (!values.empty() && !values.front().empty()) {
      // Upper-case the domain for easy checking
      String domain = values.front().toUpper();
      
      // Try and determine the domain
#ifdef LIBAISUTIL_HAVE_SOCKET_IPV4_TCP
      if ((domain == "IPV4") || (domain == "TCP/IPV4")) {
# ifdef KINE_DEBUG
	 debug("ListenerConfig::classHandler() - TCP/IPv4 socket chosen");
# endif
	 domainType = DOMAIN_IPV4;
      } else 
#endif
#ifdef LIBAISUTIL_HAVE_SOCKET_IPV6_TCP
      if ((domain == "IPV6") || (domain == "TCP/IPV6")) {
# ifdef KINE_DEBUG
	 debug("ListenerConfig::classHandler() - TCP/IPv6 socket chosen");
# endif
	 domainType = DOMAIN_IPV6;
      } else
#endif
#ifdef LIBAISUTIL_HAVE_SOCKET_IPX_SPX
      if ((domain == "IPX") || (domain == "SPX") || (domain == "IPX/SPX")) {
# ifdef KINE_DEBUG
	 debug("ListenerConfig::classHandler() - IPX/SPX socket chosen");
# endif
	 domainType = DOMAIN_IPX;
      } else
#endif
#ifdef LIBAISUTIL_HAVE_SOCKET_UNIX
      if (domain == "UNIX") {
# ifdef KINE_DEBUG
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
#ifdef LIBAISUTIL_HAVE_SOCKET_IPV4_TCP
      if (false) { // some check needs to go here..
# ifdef KINE_DEBUG
	 debug("ListenerConfig::classHandler() - "
	       "TCP/IPv4 socket determined from address");
# endif
	 domainType = DOMAIN_IPV4;
      } else
#endif
#ifdef LIBAISUTIL_HAVE_SOCKET_IPV6_TCP
      if (false) { // some check needs to go here..
# ifdef KINE_DEBUG
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
   
#ifdef LIBAISUTIL_HAVE_SOCKET_UNIX
   // If the socket type is unix - in that case we don't care about ports.
   if (domainType == DOMAIN_UNIX) {
      // Make sure the address exists!
      if (config.varAddress.empty()) {
	 // Complain
	 errString = "UNIX sockets require a filename for an address";
	 return false;
      }
      
# ifdef KINE_DEBUG
      debug("ListenerConfig::classHandler() - Creating a UNIX socket...");
# endif
      // Create the socket and set it up
      AISutil::SocketUNIX *socket = new AISutil::SocketUNIX();
      if (!config.setupSocket(*socket, errString)) {
	 return false;
      }
      
      // Add it to the list
      (dataClass.*((ListenerList AISutil::ConfigData::*)dataVariable)).listeners.
	push_front(new Listener(*socket, config.varListenBacklog, flags));
      return true;
   }
#endif
   
   // Try and convert the port number over (note the base variable on strtol())
   char *portEndPtr = 0;
   int port = strtol(config.varPort.c_str(), &portEndPtr, 0);
   bool lookupNames = false;
   
   // Check if that worked
   if (*portEndPtr != 0) {
      /* Mark this boolean true, which tells us (later) that this port needs to
       * be looked up through the getservent() suite of functions, which
       * usually means doing a lookup from /etc/services
       */
      lookupNames = true;
   } else {
      // Check the port is greater than 0
      if (port <= 0) {
	 errString = "Invalid port number '" + config.varPort + '\'';
	 return false;
      }
   }

   // If we are looking up names, make the protocols file stay open
   if (lookupNames) {
#ifdef LIBAISUTIL_HAVE_SOCKET_IPX_SPX
      // If the socket domain type is IPX, we cannot accept this.
      if (domainType == DOMAIN_IPX) {
	 errString = 
	   "Unable to look up the given port name, you must specify a number";
	 return false;
      };
#endif

      setservent(1);
   }
   
   // Too many errors for the one poor little error string?
   unsigned int manyErrors = 0;
   
   // We will use this if we are looking up names..
   servent* serviceEntry = 0;
   
   // This will cause us to loop through the protocols list, if necessary
   for (;;) {

      // Do we need to look up a name?
      if (lookupNames) {
	 // Loop until we find something useful..
	 while ((serviceEntry = getservent()) != 0) {
	    // First, make sure the protocol is the one we are looking for...
	    if (!strcasecmp(serviceEntry->s_proto, tcpProtocolName)) {
	       // Check the primary name..
	       if (!strcasecmp(serviceEntry->s_name, config.varPort.c_str())) {
		  // Set the port..
		  port = ntohs(serviceEntry->s_port);
	       } else {
		  // Loop to check the aliases (if any) for a match
		  for (unsigned int i = 0; serviceEntry->s_aliases[i] != 0;
		       i++) {
		     // Check for a match..
		     if (!strcasecmp(serviceEntry->s_aliases[i],
				     config.varPort.c_str())) {
			port = ntohs(serviceEntry->s_port);
			break;
		     }
		  }
	       }
	    }
	    
	    // If we got a valid port, break the loop
	    if (port > 0) {
	       break;
	    }
	 }
	 
	 // If the loop returned with no service data, break the loop..
	 if (serviceEntry == 0) {
	    // Break out of the loop..
	    break;
	 }
      }
      
#ifdef KINE_DEBUG
      debug("Port discovered was " + String::convert(port));
#endif
      
      /* Okay, well the port number was valid, and being a single number we
       * will only need to create ONE instance of this listener, rather than
       * look up the port(s) and potentially create multiple instances.
       */
      AISutil::Socket *socket = 0;
#ifdef LIBAISUTIL_HAVE_SOCKET_IPV4_TCP
      if (domainType == DOMAIN_IPV4) {
	 socket = new AISutil::SocketIPv4TCP();
      } else
#endif   
#ifdef LIBAISUTIL_HAVE_SOCKET_IPV6_TCP
      if (domainType == DOMAIN_IPV6) {
         socket = new AISutil::SocketIPv6TCP();
      } else
#endif
#ifdef LIBAISUTIL_HAVE_SOCKET_IPX_SPX
      if (domainType == DOMAIN_IPX) {
         socket = new AISutil::SocketIPXSPX();
      } else
#endif
      { 
	 // explode? we should never reach here to be honest..
#ifdef KINE_DEBUG
	 debug("Woops, we hit somewhere we shouldn't have.");
#endif
	 abort();
      };
   
#ifdef KINE_DEBUG_ASSERT
      // Make sure we are not insane - we must have gotten a socket here.
      assert(socket != 0);
#endif
      
      // Configure the socket...
      if (!config.setupSocket(*socket, errString, port)) {
	 // If we are not looking up names, terminate unhappily now..
	 if (!lookupNames) {
	    return false;
	 } else {
	    manyErrors++;
	 }
      }
      
      // Add it to the list
      (dataClass.*((ListenerList ConfigData::*)dataVariable)).listeners.
	push_front(new Listener(*socket, config.varListenBacklog, flags));
      
      // If we are not looping through the names, break out now..
      if (!lookupNames) {
	 break;
      } else {
	 /* We might be looping again.. This time, set the port -2 so we know
	  * this is not the first port we have discovered from the services
	  * list..
	  */
	 port = -2;
      }
   }

   // If we are looping through service names, we need to close the database
   if (lookupNames) {
      endservent();
   }
   
   // Check if there were errors we couldn't deal with
   if (manyErrors > 1) {
      errString = 
	"There are some errors, some or all ports many not have been opened.";
      return false;
   } else if (manyErrors == 1) {
      return false;
   }
   
   // All is well
   return true;
}
