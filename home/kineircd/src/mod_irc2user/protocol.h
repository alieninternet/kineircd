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
 */

#ifndef _SRC_MOD_IRC2USER_PROTOCOL_H_
# define _SRC_MOD_IRC2USER_PROTOCOL_H_ 1

# include <sstream>
# include <kineircd/config.h>

# include "libkineircd_irc2/protocol.h"
# include "libkineircd_irc2/numerics.h"
# include "mod_irc2user/user.h"

namespace Kine {
   namespace mod_irc2user {
      class Protocol : public Kine::LibIRC2::Protocol {
       private:
	 User user;				// The user for this instance
	 
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

	 // Send a numeric (various forms, this could probably be done better)
	 void sendNumeric(const LibIRC2::Numerics::numeric_type numeric)
	   {
	      sendMessage(config().getOptionsServerName(),
			  user.getNickname(), numeric);
	   };
	 template <class Ta>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa)
	     {
		std::ostringstream output;
		output << ':' << pa;
		sendMessage(config().getOptionsServerName(),
			    user.getNickname(), numeric, output.str());
	     };
	 template <class Ta, class Tb>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb)
	     {
		std::ostringstream output;
		output << pa << " :" << pb;
		sendMessage(config().getOptionsServerName(),
			    user.getNickname(), numeric, output.str());
	     };
	 template <class Ta, class Tb, class Tc>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc)
	     {
		std::ostringstream output;
		output << pa << ' ' << pb << " :" << pc;
		sendMessage(config().getOptionsServerName(),
			    user.getNickname(), numeric, output.str());
	     };
	 template <class Ta, class Tb, class Tc, class Td>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd)
	     {
		std::ostringstream output;
		output << pa << ' ' << pb << ' ' << pc << " :" << pd;
		sendMessage(config().getOptionsServerName(),
			    user.getNickname(), numeric, output.str());
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe)
	     {
		std::ostringstream output;
		output << pa << ' ' << pb << ' ' << pc << ' ' << pd << 
		  " :" << pe;
		sendMessage(config().getOptionsServerName(),
			    user.getNickname(), numeric, output.str());
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te, class Tf>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe, const Tf& pf)
	     {
		std::ostringstream output;
		output << pa << ' ' << pb << ' ' << pc << ' ' << pd <<
		  ' ' << pe << " :" << pf;
		sendMessage(config().getOptionsServerName(),
			    user.getNickname(), numeric, output.str());
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te, class Tf,
	           class Tg>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe, const Tf& pf,
			    const Tg& pg)
	     {
		std::ostringstream output;
		output << pa << ' ' << pb << ' ' << pc << ' ' << pd <<
		  ' ' << pe << ' ' << pf << " :" << pg;
		sendMessage(config().getOptionsServerName(),
			    user.getNickname(), numeric, output.str());
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te, class Tf,
	           class Tg, class Th>
	   void sendNumeric(const LibIRC2::Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe, const Tf& pf,
			    const Tg& pg, const Th& ph)
	     {
		std::ostringstream output;
		output << pa << ' ' << pb << ' ' << pc << ' ' << pd <<
		  ' ' << pe << ' ' << pf << ' ' << pg << " :" << ph;
		sendMessage(config().getOptionsServerName(),
			    user.getNickname(), numeric, output.str());
	     };
      }; // class Protocol
   }; // namespace mod_irc2user
}; // namespace Kine
   
#endif // _SRC_MOD_IRC2USER_PROTOCOL_H_
   
