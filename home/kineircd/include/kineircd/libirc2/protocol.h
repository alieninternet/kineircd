/* $Id$
 * The IRC-2 protocol suite base class
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

#ifndef _SRC_LIBKINEIRCD_IRC2_PROTOCOL_H_
# define _SRC_LIBKINEIRCD_IRC2_PROTOCOL_H_ 1

# include <queue>
# include <vector>
# include <string>
# include <aisutil/string/string.h>
# include <kineircd/protocol.h>

namespace Kine {
   namespace LibIRC2 {
      class Protocol : public Kine::Protocol {
       protected:
	 // The type of a 'parameter list'
	 typedef std::vector <AISutil::String> parameters_type;

	 // Our input buffer..
	 std::string buffer;

	 // The output data queue
	 std::queue <std::string> outputQueue;
	 
       private:
	 // Appropriately parse a protocol message
	 virtual void parseMessage(const std::string& origin,
				   const std::string& command,
				   const std::string& destination,
				   const parameters_type& parameters) = 0;

	 // Break up a protocol message into its components
	 virtual void parseLine(const std::string& line);

       protected:
	 // Constructor
	 Protocol(Kine::Connection& c)
	   : Kine::Protocol(c)
	   {};
	 
       public:
	 // Destructor
	 virtual ~Protocol(void)
	   {};
	 
	 // Handle incoming data
	 virtual void handleInput(std::stringstream& data);
	 
	 // Remove up to the amount of octets given from the output queue
	 std::string withdrawOutput(AISutil::Socket::blockSize_type amount);
	 
	 // Return true should there be anything in the output queue to send
	 bool moreOutput(void) const
	   { return (!outputQueue.empty()); };
      };
   }; // namespace LibIRC2
}; // namespace Kine
   
#endif // _SRC_LIBKINEIRCD_IRC2_PROTOCOL_H_
   
