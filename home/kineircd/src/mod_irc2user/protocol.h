/* $Id$
 * The IRC-2 user protocol class
 * 
 * Copyright (c) 2001,2002 Simon Butcher <pickle@alien.net.au>
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

#ifndef _SRC_MOD_IRC2USER_PROTOCOL_H_
# define _SRC_MOD_IRC2USER_PROTOCOL_H_ 1

# include "libkineircd_irc2/protocol.h"

namespace Kine {
   namespace mod_irc2user {
      class Protocol : public Kine::LibIRC2::Protocol {
       private:
	 // Appropriately parse a protocol message
	 void parseMessage(const std::string& origin,
			   const std::string& command,
			   const std::string& destination,
			   const Kine::LibIRC2::Protocol::parameters_type&
			   parameters)
	   {};
	 
       public:
	 // Constructor
	 Protocol(const Kine::Registrant& registrant,
		  Kine::Connection& connection, std::string& inputQueue,
		  std::string& outputQueue)
	   : Kine::LibIRC2::Protocol(connection, inputQueue, outputQueue)
	   {};
	 
	 // Destructor
	 ~Protocol(void)
	   {};
      };
   }; // namespace mod_irc2user
}; // namespace Kine
   
#endif // _SRC_MOD_IRC2USER_PROTOCOL_H_
   
