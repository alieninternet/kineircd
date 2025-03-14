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

#ifndef _SRC_MOD_IRC2USER_PROTOCOLINFO_H_
# define _SRC_MOD_IRC2USER_PROTOCOLINFO_H_ 1

# include <kineircd/protocolinfo.h>

# include "mod_irc2user/protocol.h"

namespace Kine {
   namespace mod_irc2user {
      // The IRC-2 user protocol information
      class ProtocolInfo : public Kine::ProtocolInfo {
       public:
	 // Constructor
	 ProtocolInfo(void)
	   : Kine::ProtocolInfo(Kine::ProtocolInfo::Type::USER, "IRC/2")
	   {};
	 
	 // Destructor
	 ~ProtocolInfo(void)
	   {};
	 
	 Kine::Protocol::Base* const
	   createProtocol(const Kine::Registrant& r,
			  Kine::Connection& c,
			  const std::string& iq,
			  const std::string& oq)
	   {
	      return new Kine::mod_irc2user::Protocol(r, c, iq, oq);
	   };
      };
   }; // namespace mod_irc2user
}; // namespace Kine
   
#endif // _SRC_MOD_IRC2USER_PROTOCOLINFO_H_
   
