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

#ifndef _SRC_MOD_IRC2REGISTRAR_PROTOCOL_H_
# define _SRC_MOD_IRC2REGISTRAR_PROTOCOL_H_ 1

# include <string>
# include <kineircd/listener.h>
# include <kineircd/registrant.h>
# include <kineircd/config.h>
# include <kineircd/irc2/protocol.h>
# include <kineircd/irc2/numerics.h>

# define KINE_MOD_REGISTRAR_FUNCTION(x) \
     void x(const Kine::LibIRC2::Protocol::parameters_type& parameters)

namespace Kine {
   namespace mod_irc2registrar {
      class Protocol : public LibIRC2::Protocol {
       private:
	 const Listener& listener;		// The listener who invoked us
	 
	 Registrant registrantData;		// Collected registrant info
	 
	 struct RegistrationType { // <=- Should be namespace?
	    enum type {
	       NONE = 0,			// No type, yet
	       IIRCN,				// An IIRC network connection
	       SERVER,				// A server connection
	       SERVICE,				// A service connection
	       USER				// A client or user connection
	    };
	 };
	 RegistrationType::type
	   registrationType;	// Type of registration
	 
	 unsigned char pongsLeft;		// Number of pongs left
	 std::string pongMatch;			// Pong string to match
	 
	 // The type of a handler, for handy use later..
	 typedef KINE_MOD_REGISTRAR_FUNCTION(handler_type);
      
	 // A list of command parsing functions
	 struct commandTable_type {
	    const char* const command;			// Command name
	    handler_type Protocol::* const handler;	// Parser/Handler
	 } static const commandTable[];
	 
	 // Send a numeric
	 void sendNumeric(const LibIRC2::Numerics::numeric_type numeric)
	   {
	      sendMessageFrom(config().getOptionsServerName(), numeric,
			      (registrantData.name.empty() ? 
			       '*' : registrantData.name),
			      "");
	   };
	 
	 void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			  const char* const data)
	   {
	      sendMessageFrom(config().getOptionsServerName(), numeric,
			      (registrantData.name.empty() ? 
			       '*' : registrantData.name),
			      data);
	   };
	 
	 // Send an error message and disconnect
	 void sendError(const std::string& error)
	   {
	      sendMessage("ERROR", ':' + error);
	      connection.goodbye();
	   };
	 
	 // Send a ping with some unpredictable data
	 void sendPing(void);
	 
	 // Appropriately parse a protocol message
	 void parseMessage(const std::string& origin, 
			   const std::string& destination,
			   const std::string& command,
			   const Kine::LibIRC2::Protocol::parameters_type&
			   parameters);
	 
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
	 Protocol(Connection& c, const Listener& l)
	   : LibIRC2::Protocol(c),
	     listener(l),
	     registrationType(RegistrationType::NONE),
	     pongsLeft(config().getOptionsRegistrarUserPingProbeCount())
	   {};
      
	 // Destructor
	 ~Protocol(void)
	   {};
      }; // class Protocol
   }; // namespace mod_irc2registrar
}; // namespace Kine


// Language related macros
# define GETLANG(n) \
   Kine::languages().get("en", \
			 Language::tagMap[Language::n].tagID)


#endif // _SRC_MOD_PROTOCOL_REGISTRAR_H_

