/* $Id$
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

#ifndef _SRC_MOD_HTTP_PROTOCOLINFO_H_
# define _SRC_MOD_HTTP_PROTOCOLINFO_H_ 1

# include <kineircd/protocolinfo.h>

# include "module/protocol.h"

namespace Kine {
   namespace mod_http {
      class ProtocolInfo : public Kine::ProtocolInfo {
       public:
	 // Constructor
	 ProtocolInfo(void)
	   : Kine::ProtocolInfo(Kine::ProtocolInfo::Type::PRIMARY, "HTTP/*")
	   {};
	 
	 // Destructor
	 ~ProtocolInfo(void)
	   {};
	 
	 Kine::Protocol* const createProtocol(Kine::Connection& c,
					      const Kine::Listener& l)
	   { return new Kine::mod_http::Protocol(c); };
      }; // class ProtocolInfo
   }; // namespace mod_http
}; // namespace Kine
   
#endif // _SRC_MOD_HTTP_PROTOCOLINFO_H_
   
