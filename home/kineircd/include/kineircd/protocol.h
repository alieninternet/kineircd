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

#ifndef _INCLUDE_KINEIRCD_PROTOCOL_H_
# define _INCLUDE_KINEIRCD_PROTOCOL_H_ 1

# include <string>
# include <sstream>
# include <cstdlib>
# include <aisutil/socket/socket.h>
# include <kineircd/connection.h>

namespace Kine {
   //! Protocol base classes
   namespace Protocol {
      //! The protocol base class
      class Base {
       public:
	 //! The type of a message counter
	 typedef unsigned long messageCount_type;
	 
	 //! An unknown number of messages
	 static const messageCount_type unknownMessageCount =
	   ((messageCount_type)-1);
	 
       protected:
	 //! Constructor
	 explicit Base(void)
	   {};
	 
       public:
	 // Destructor
	 virtual ~Base(void)
	   {};
	 
	 // Handle incoming data
	 virtual void handleInput(std::stringstream& data) = 0;
	 
	 // Remove up to the amount of octets given from the output queue
	 virtual const std::string 
	   withdrawOutput(const AIS::Util::Socket::blockSize_type amount) = 0;
	 
	 // Return true should there be anything in the output queue to send
	 virtual const bool moreOutput(void) const = 0;
	 
	 
	 //! Return the number of messages sent through this protocol
	 virtual const messageCount_type getSentMessageCount(void) const
	   { return unknownMessageCount; };
	 
	 //! Return the number of messages received through this protocol
	 virtual const messageCount_type getReceivedMessageCount(void) const
	   { return unknownMessageCount; };
	 
	 
	 //! Return some sort of official name of this protocol
	 virtual const char* const getProtocolName(void) const = 0;
	 
	 //! Return some sort of identifying name for this instance
	 virtual const std::string* const getIdentifyingName(void) const
	   { return 0; };
      }; // class Base
      
      
      //! Generic protocol input base
      class Input : virtual public Base {
       protected:
	 //! Constructor
	 explicit Input(void)
	   {};
	 
       public:
	 //! Destructor
	 virtual ~Input(void)
	   {};
      }; // class Input
      
      
      //! Generic protocol output base
      class Output : virtual public Base {
       protected:
	 //! The specific connection running this instance of the protocol
	 Connection& connection;
	 
	 
	 //! Constructor
	 explicit Output(Connection& _connection)
	   : connection(_connection)
	   {};
	 
       public:
	 //! Destructor
	 virtual ~Output(void)
	   {};
      }; // class Output
      
      
      //! Generic protocol including both input and output components
      class Protocol : public Input, public Output {
       protected:
	 //! Constructor
	 explicit Protocol(Connection& _connection)
	   : Output(_connection)
	   {};
	 
       public:
	 //! Destructor
	 virtual ~Protocol(void)
	   {};
      }; // class Protocol
   }; // namespace Protocol
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_PROTOCOL_H_

