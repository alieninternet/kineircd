/* $Id$
 * Handle registration (before handling a real protocol)
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

#ifndef _SRC_LIBKINEIRCD_REGISTRAR_H_
# define _SRC_LIBKINEIRCD_REGISTRAR_H_ 1

# include <aisutil/string/tokens.h>

# include "kineircd/listener.h"
# include "kineircd/registrant.h"
# include "kineircd/config.h"
# include "libkineircd_irc2/protocol.h"
# include "libkineircd/regnumerics.h"

# define KINE_LIB_REGISTRAR_FUNCTION(x) \
     void x(AISutil::StringTokens& line)

namespace Kine {
   class Registrar : public LibIRC2::Protocol {
    private:
      Listener& listener;			// The listener who invoked us

      Registrant registrantData;		// Collected registrant info
      
      struct RegistrationType { // <=- Should be namespace?
	 enum type {
	    NONE = 0,				// No type, yet
	    IIRCN,				// An IIRC network connection
	    SERVER,				// A server connection
	    SERVICE,				// A service connection
	    USER				// A client or user connection
	 };
      };
      RegistrationType::type registrationType;	// Type of registration
      
      unsigned char pongsLeft;			// Number of pongs left
      std::string pongMatch;			// Pong string to match
      
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
		       const char* const data);

      // Send an error message and disconnect
      void sendError(const char* const error);
      
      // Send a ping with some unpredictable data
      void sendPing(void);

      // Appropriately parse a protocol message
      void parseLine(const std::string& line);

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
          pongsLeft(config().getOptionsRegistrarUserPingProbeCount())
	{};
      
      // Destructor
      ~Registrar(void)
	{};
   };
}; // namespace Kine
   
#endif // _SRC_LIBKINEIRCD_REGISTRAR_H_

