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

#ifndef _SRC_LIB_REGISTER_H_
# define _SRC_LIB_REGISTER_H_ 1

# include "kineircd/protocol.h"
# include "kineircd/str.h"

# define KINE_LIB_REGISTER_FUNCTION(x) \
     void x(Kine::StringTokens& line)

namespace Kine {
   class Register : public Protocol {
    private:
      String buffer;				// Our buffer..
      
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
      
      String password;				// Logon password
      String nickname;				// Nickname (client/service)
      String username;				// Username (client/server)
      String hostname;				// Hostname
      String realname;				// Realname/Description
      String distribution;			// Distribution range (service)
      String modes;				// Modes for next handler
      String protocol;				// Protocol details
      long startStamp;				// Received start time-stamp
      long linkStamp;				// Received link time-stamp

      unsigned char pongsLeft;			// Number of pongs left
      String pongMatch;				// Pong string to match
      
      // The type of a handler, for handy use later..
      typedef KINE_LIB_REGISTER_FUNCTION(handler_type);
      
      // A list of command parsing functions
      struct commandTable_type {
	 const char* const command;			// Command name
	 const handler_type Register::* const handler;	// Parser/Handler
      } static const commandTable[];

      // Appropriately parse a line of protocol
      void parseLine(const String& line);

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
      Register(Connection& c);
      
      // Destructor
      ~Register(void)
	{};
      
      // Handle incoming data
      void handleInput(std::stringstream& data);
   };
};
   
#endif // _SRC_LIB_REGISTER_H_
   
