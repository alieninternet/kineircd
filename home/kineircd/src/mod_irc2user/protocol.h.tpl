[+ AutoGen5 template h +]
/* $Id$
 * The IRC-2 user protocol class
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
 *
 *
[+(dne " * ")+]
 */

#ifndef _SRC_MOD_IRC2USER_PROTOCOL_H_
# define _SRC_MOD_IRC2USER_PROTOCOL_H_ 1

# include <sstream>
# include <kineircd/config.h>
# include <kineircd/irc2/protocol.h>
# include <kineircd/irc2/numerics.h>

# include "mod_irc2user/user.h"


// Handy definition thingy
# define IRC2USER_COMMAND_HANDLER(x) \
     void x(const Kine::LibIRC2::Protocol::parameters_type& parameters)


namespace Kine {
   namespace mod_irc2user {
      class Protocol : public Kine::LibIRC2::Protocol {
       public:
	 // Convenient type-definition of handler functions
	 typedef IRC2USER_COMMAND_HANDLER(handler_type);
	 
       private:
	 User user;				// The user for this instance

	 messageSize_type maxMessageSize;	// Maximum message size

	 // Appropriately parse a protocol message
	 void parseMessage(const std::string& origin,
			   const std::string& destination,
			   const std::string& command,
			   const Kine::LibIRC2::Protocol::parameters_type&
			   parameters);
	 
       public:
	 // Constructor (init the connection)
	 Protocol(const Kine::Registrant& registrant,
		  Kine::Connection& connection, std::string& inputQueue,
		  std::string& outputQueue);
	 
	 // Destructor
	 ~Protocol(void)
	   {};

	 // Send a numeric
	 void sendNumeric(const LibIRC2::Numerics::numeric_type numeric)
	   {
	      LibIRC2::Protocol::
	        sendMessageTo(config().getOptionsServerName(),
			      user.getNickname(), numeric);
	   };
	 template <class Ta>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa)
	     {
		LibIRC2::Protocol::
		  sendMessageTo(config().getOptionsServerName(),
			        user.getNickname(), numeric,
				pa);
	     };
	 template <class Ta, class Tb>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb)
	     {
		LibIRC2::Protocol::
		  sendMessageTo(config().getOptionsServerName(),
			        user.getNickname(), numeric,
			        pa, pb);
	     };
	 template <class Ta, class Tb, class Tc>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc)
	     {
		LibIRC2::Protocol::
		  sendMessageTo(config().getOptionsServerName(),
			        user.getNickname(), numeric,
			        pa, pb, pc);
	     };
	 template <class Ta, class Tb, class Tc, class Td>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd)
	     {
		LibIRC2::Protocol::
		  sendMessageTo(config().getOptionsServerName(),
			        user.getNickname(), numeric,
			        pa, pb, pc, pd);
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe)
	     {
		LibIRC2::Protocol::
		  sendMessageTo(config().getOptionsServerName(),
			        user.getNickname(), numeric,
			        pa, pb, pc, pd, pe);
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te, class Tf>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe, const Tf& pf)
	     {
		LibIRC2::Protocol::
		  sendMessageTo(config().getOptionsServerName(),
			        user.getNickname(), numeric,
			        pa, pb, pc, pd, pe, pf);
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te, class Tf,
	           class Tg>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe, const Tf& pf,
			    const Tg& pg)
	     {
		LibIRC2::Protocol::
		  sendMessageTo(config().getOptionsServerName(),
			        user.getNickname(), numeric,
			        pa, pb, pc, pd, pe, pf, pg);
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te, class Tf,
	           class Tg, class Th>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe, const Tf& pf,
			    const Tg& pg, const Th& ph)
	     {
		LibIRC2::Protocol::
		  sendMessageTo(config().getOptionsServerName(),
			        user.getNickname(), numeric,
			        pa, pb, pc, pd, pe, pf, pg, ph);	
	     };
	 
	 // Command handlers[+FOR commands+]
	 handler_type handle[+command+];[+ENDFOR+]
      }; // class Protocol
   }; // namespace mod_irc2user
}; // namespace Kine
   
#endif // _SRC_MOD_IRC2USER_PROTOCOL_H_
   
