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

#ifndef _SRC_LIB_LISTENER_CONFIG_H_
# define _SRC_LIB_LISTENER_CONFIG_H_ 1

# include "kineircd/listener.h"
# include "kineircd/listenerlist.h"
# include "kineircd/configparser.h"
# include "kineircd/configdata.h"
# include "kineircd/str.h"

namespace Kine {
   class ListenerConfig : public ConfigData {
    private:
      // The protocol used to locate any service's name in getservbyname()
      static const char* tcpProtocolName;
      
      // The list of definitions for the configuration handler
      static const ConfigParser::defTable_type classDefs;

      // The address to bind to (optional)
      String varAddress;
      
      // Registration allowances
      bool varAllowNetworks;
      bool varAllowServers;
      bool varAllowServices;
      bool varAllowUsers;

      // The port to bind to
      String varPort;

      // Should this port be configured as secure if possible? (SSL etc)
      bool varSecure;
      
      // A helper to set the socket up
      bool setupSocket(Socket& socket, String& errString, int port = 0);
      
    public:
      ListenerConfig(void);

      // The class handler for the configuration class
      static CONFIG_CLASS_HANDLER(classHandler);
   };
};
   
#endif // _SRC_LIB_LISTENER_CONFIG_H_
   
