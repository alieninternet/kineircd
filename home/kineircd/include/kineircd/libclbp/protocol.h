/* $Id$
 * 
 * Copyright (c) 2001,2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2002,2003 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_CLBP_PROTOCOL_H_
# define _INCLUDE_KINEIRCD_CLBP_PROTOCOL_H_ 1

# include <queue>
# include <string>
# include <sstream>
# include <kineircd/protocol.h>


namespace Kine {
   namespace LibCLBP {
      //! The command line based protocol (CLBP) base class
      class Protocol : public Kine::Protocol {
       protected:
	 // Our input queue..
	 std::string inputQueue;

	 // The output data queue
	 std::queue < std::string > outputQueue;

	 
	 //! Constructor
	 Protocol(Kine::Connection& c)
	   : Kine::Protocol(c)
	   {};

	 //! Constructor (for migrating I/O queues)
	 Protocol(Kine::Connection& c, std::string& iq, std::string& oq);
	 
       public:
	 //! Destructor
	 virtual ~Protocol(void)
	   {};

       private:
	 //! Parse a line (you must replace this)
	 virtual void parseLine(const std::string& line) = 0;
	 
       public:
	 //! Handle incoming data
	 void handleInput(std::stringstream& data);
	 
	 //! Remove up to the amount of octets given from the output queue
	 std::string withdrawOutput(AIS::Util::Socket::blockSize_type amount);
	 
	 //! Return true should there be anything in the output queue to send
	 bool moreOutput(void) const
	   { return (!outputQueue.empty()); };
      };
   }; // namespace LibCLBP
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_CLBP_PROTOCOL_H_
   
