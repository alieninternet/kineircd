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

#ifndef _SRC_MOD_HTTP_PROTOCOL_H_
# define _SRC_MOD_HTTP_PROTOCOL_H_ 1

# include <kineircd/clbp/protocol.h>

namespace Kine {
   namespace mod_http {
      // The IRC-2 user protocol class
      class Protocol : public Kine::LibCLBP::Protocol {
       private:
	 static const char* const protocolName;	// An 'official' name for us
	 
       public:
	 // Constructor (new connection)
	 Protocol(Kine::Connection& c)
	   : Kine::LibCLBP::Protocol(c)
	   {};
	 
	 // Constructor (migration)
	 Protocol(Kine::Connection& c, std::string& iq, std::string& oq)
	   : Kine::LibCLBP::Protocol(c, iq, oq)
	   {};
	 
	 // Destructor
	 ~Protocol(void)
	   {};

	 
	 // Return some sort of official name of this protocol
	 const char* const getProtocolName(void) const
	   { return protocolName; };
	 
	 
       private:
	 // Process an incoming line of data
	 void parseLine(const std::string& line);
      }; // class Protocol
   }; // namespace mod_http
}; // namespace Kine

#endif // _SRC_MOD_HTTP_PROTOCOL_H_
   
