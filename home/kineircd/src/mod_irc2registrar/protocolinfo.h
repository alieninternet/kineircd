/* $Id$
 * The IRC-2 registrar protocol information
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
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

#ifndef _SRC_MOD_IRC2REGISTRAR_PROTOCOLINFO_H_
# define _SRC_MOD_IRC2REGISTRAR_PROTOCOLINFO_H_ 1

# include <kineircd/protocolinfo.h>

# include "mod_irc2registrar/protocol.h"

namespace Kine {
   namespace mod_irc2registrar {
      class ProtocolInfo : public Kine::ProtocolInfo {
       public:
	 // Constructor
	 ProtocolInfo(void)
	   : Kine::ProtocolInfo(Kine::ProtocolInfo::Type::PRIMARY, "IRC/2")
	   {};
	 
	 // Destructor
	 ~ProtocolInfo(void)
	   {};
	 
	 Kine::Protocol* const createProtocol(Kine::Connection& c,
					      const Kine::Listener& l)
	   {
	      return new Kine::mod_irc2registrar::Protocol(c, l);
	   };
      };
   }; // namespace mod_irc2registrar
}; // namespace Kine
   
#endif // _SRC_MOD_IRC2REGISTRAR_PROTOCOLINFO_H_
   
