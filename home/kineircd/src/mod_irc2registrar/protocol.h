/* $Id$
 * The protocol base class
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

#ifndef _SRC_LIB_REGISTRAR_H_
# define _SRC_LIB_REGISTRAR_H_ 1

# include <queue>
# include <string>
# include <aisutil/string/string.h>
# include <aisutil/string/tokens.h>

# include "kineircd/protocol.h"
# include "kineircd/listener.h"
# include "regnumerics.h"

# define KINE_LIB_REGISTRAR_FUNCTION(x) \
     void x(AISutil::StringTokens& line)

namespace Kine {
   class Registrar : public Protocol {
    private:
      Listener& listener;			// The listener who invoked us
      
      AISutil::String buffer;			// Our buffer..
      
      std::queue <std::string> outputQueue;	// Output data queue
      
      struct RegistrationType { // <=- Should be namespace?
	 enum type {
	    NONE = 0,				// No type, yet
	    CLIENT,				// A client or user connection
	    IIRCN,				// An IIRC network connection
	    SERVER,				// A server connection
	    SERVICE				// A service connection
	 };
      };
      RegistrationType::type registrationType;	// Type of registration
      
      AISutil::String password;			// Logon password
      AISutil::String nickname;			// Nickname (client/service)
      AISutil::String username;			// Username (client/server)
      AISutil::String hostname;			// Hostname
      AISutil::String realname;			// Realname/Description
      AISutil::String distribution;		// Distribution range (service)
      AISutil::String modes;			// Modes for next handler
      AISutil::String protocol;			// Protocol details
      long startStamp;				// Received start time-stamp
      long linkStamp;				// Received link time-stamp

      unsigned char pongsLeft;			// Number of pongs left
      AISutil::String pongMatch;		// Pong string to match
      
      // The type of a handler, for handy use later..
      typedef KINE_LIB_REGISTRAR_FUNCTION(handler_type);
      
      // A list of command parsing functions
      struct commandTable_type {
	 const char* const command;			// Command name
	 const handler_type Registrar::* const handler;	// Parser/Handler
      } static const commandTable[];

      // Send a numeric
      void sendNumeric(const RegistrationNumerics::numeric_type numeric);
      void sendNumeric(const RegistrationNumerics::numeric_type numeric,
		       const char* data);
      
      // Appropriately parse a line of protocol
      void parseLine(const AISutil::String& line);

      // Protocol commands
      handler_type parseCAPAB;
      handler_type parseIIRCN;
      handler_type parseNICK;
      handler_type parsePASS;
      handler_type parsePONG;
      handler_type parseQUIT;
      handler_type parseSERVER;
      handler_type parseSERVICE;
      handler_type parseUSER;
      
    public:
      // Constructor
      Registrar(Connection& c, Listener& l)
	: Protocol(c),
          listener(l),
          registrationType(RegistrationType::NONE),
          pongsLeft(0)
	{};
      
      // Destructor
      ~Registrar(void)
	{};
      
      // Handle incoming data
      void handleInput(std::stringstream& data);
      
      // Remove up to the amount of octets given from the output queue
      std::string withdrawOutput(AISutil::Socket::blockSize_type amount);
      
      // Return true should there be anything in the output queue to send
      bool moreOutput(void) const
	{ return (!outputQueue.empty()); };
   };
};
   
#endif // _SRC_LIB_REGISTRAR_H_
   
