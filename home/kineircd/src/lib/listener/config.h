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

#ifndef _SRC_LIBKINEIRCD_LISTENER_CONFIG_H_
# define _SRC_LIBKINEIRCD_LISTENER_CONFIG_H_ 1

# include <aisutil/config/data.h>
# include <aisutil/string/string.h>

# include "kineircd/listener.h"
# include "kineircd/listenerlist.h"

namespace Kine {
   // Listener configuration parser helper class
   class ListenerConfig : public AISutil::ConfigData {
    private:
      // The protocol used to locate any service's name in getservbyname()
      static const char* tcpProtocolName;
      
      // The list of definitions for the configuration handler
      static const AISutil::ConfigParser::defTable_type classDefs;

      // The address to bind to (optional)
      AISutil::String varAddress;
      
      // Registration allowances
      bool varAllowNetworks;
      bool varAllowServers;
      bool varAllowServices;
      bool varAllowUsers;

      // The backlog number to send listen()
      unsigned short varListenBacklog;
      
      // The port to bind to
      AISutil::String varPort;

      // Should this port be configured as secure if possible? (SSL etc)
      bool varSecure;

      // A helper to set the socket up
      bool setupSocket(AISutil::Socket& socket, AISutil::String& errString,
		       int port = 0);
      
    public:
      // Constructor
      ListenerConfig(void);

      // Destructor
      ~ListenerConfig(void)
	{};
      
      // The class handler for the configuration class
      static LIBAISUTIL_CONFIG_CLASS_HANDLER(classHandler);
   };
};
   
#endif // _SRC_LIBKINEIRCD_LISTENER_CONFIG_H_

