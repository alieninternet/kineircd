/* $Id$
 * The protocol information class
 * 
 * Copyright (c) 2001,2002 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_PROTOCOLINFO_H_
# define _INCLUDE_KINEIRCD_PROTOCOLINFO_H_ 1

# include <cstring>
# include <kineircd/protocolname.h>
# include <kineircd/protocol.h>
# include <kineircd/listener.h>
# include <kineircd/registrant.h>

namespace Kine {
   /* ProtocolInfo class - This class 'defines' a protocol, to help the 
    * daemon, and more often the registrar determine if a particular 
    * protocol is the one the remote connection is requesting. Seemingly not
    * much information goes in here, unfortunately, due to the nature of IRC.
    * All we mainly store are the two main attributes of a protocol: its name,
    * and its type. 
    * 
    * The name and type values are kept inside the protocol name structure for
    * two reasons - firstly the programmer will most likely wish this data to
    * be static in their module. Secondly, the daemon uses this to efficiently
    * scan through its internal protocol list.
    *
    * If the registrar needs more confirmation of the protocol, it will call
    * the checkProtocolMatch() function which may be provided if it is known
    * that the protocol will not be immediately recognised by its name and
    * type (such as would occur with most server to server protocols).
    * 
    * If the registrar believes it has found a suitable match, the protocol
    * will attempt to be created using the given creation function. If the
    * protocol cannot be created, this is your last chance to back out by
    * returning a 0 value (a null pointer), otherwise the protocol created
    * will be given control of the connection.
    */
   class ProtocolInfo {
    public:
      // Constructor
      ProtocolInfo(void)
	{};
	 
      // Destructor
      virtual ~ProtocolInfo(void)
	{};
	 
      // This function checks if this protocol matches the given criterion
      virtual const bool checkProtocolMatch(const Registrant& registrant)
	{ return true; };
	 
      /* This function returns a brand new (inherited) protocol class which
       * matches the protocol described and checked here. This also passes
       * pending input and output queues, and connection information on.
       */
      virtual Protocol* const createProtocol(const Registrant& registrant,
					     Connection& connection,
					     Listener& listener,
					     std::string& inputQueue,
					     std::string& outputQueue) = 0;
   };
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_PROTOCOLINFO_H_
   
