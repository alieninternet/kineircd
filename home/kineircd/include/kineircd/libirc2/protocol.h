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

#ifndef _INCLUDE_KINEIRCD_IRC2_PROTOCOL_H_
# define _INCLUDE_KINEIRCD_IRC2_PROTOCOL_H_ 1

# include <queue>
# include <vector>
# include <string>
# include <sstream>
# include <aisutil/string/string.h>
# include <kineircd/protocol.h>
# include <kineircd/user.h>
# include <kineircd/irc2/numerics.h>

namespace Kine {
   namespace LibIRC2 {
      class Protocol : public Kine::Protocol {
       public:
	 typedef unsigned int messageSize_type;
	 const char* lineTerminator;
	 
       protected:
	 // The type of a 'parameter list'
	 typedef std::vector <AISutil::String> parameters_type;

	 // Our input queue..
	 std::string inputQueue;

	 // The output data queue
	 std::queue <std::string> outputQueue;
	 
       private:
	 // Appropriately parse a protocol message
	 virtual void parseMessage(const std::string& origin,
				   const std::string& destination,
				   const std::string& command,
				   const parameters_type& parameters) = 0;

	 // Break up a protocol message into its components
	 virtual void parseLine(const std::string& line);

       protected:
	 // Constructor
	 Protocol(Kine::Connection& c)
	   : Kine::Protocol(c),
	     lineTerminator("\r\n")
	   {};

	 // Constructor (for migrating I/O queues)
	 Protocol(Kine::Connection& c, std::string& iq, std::string& oq);

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
	 
	 // Send an appropriately formatted message, without TO/FROM fields
	 template <class Tc, class Tp>
	   void sendMessage(const Tc& command)
	   {
	      std::ostringstream output;
	      output << command << lineTerminator;
	      outputQueue.push(output.str());
	   };
	 template <class Tc, class Tp>
	   void sendMessage(const Tc& command, const Tp& parameters)
	   {
	      std::ostringstream output;
	      output << command << ' ' << parameters << lineTerminator;
	      outputQueue.push(output.str());
	   };

	 // Send a properly formatted message with TO/FROM (I don't like this)
	 template <class To, class Td, class Tc>
	   void sendMessageTo(const To& origin, const Td& destination,
			      const Tc& command)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << 
		  destination << lineTerminator;
		outputQueue.push(output.str());
	     };
	 template <class To, class Td, class Tc,
	           class Tpa>
	   void sendMessageTo(const To& origin, const Td& destination,
			      const Tc& command,
			      const Tpa& pa)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' <<
		  destination << " :" << pa << lineTerminator;
		outputQueue.push(output.str());
	     };
	 template <class To, class Td, class Tc,
	           class Tpa, class Tpb>
	   void sendMessageTo(const To& origin, const Td& destination,
			      const Tc& command,
			      const Tpa& pa, const Tpb& pb)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << 
		  destination << ' ' << pa << " :" << pb << lineTerminator;
		outputQueue.push(output.str());
	     };
	 template <class To, class Td, class Tc,
	           class Tpa, class Tpb, class Tpc>
	   void sendMessageTo(const To& origin, const Td& destination,
			      const Tc& command,
			      const Tpa& pa, const Tpb& pb, const Tpc& pc)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << 
		  destination << ' ' << pa << ' ' << pb << " :" << pc <<
		  lineTerminator;
		outputQueue.push(output.str());
	     };
	 template <class To, class Td, class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd>
	   void sendMessageTo(const To& origin, const Td& destination,
			      const Tc& command,
			      const Tpa& pa, const Tpb& pb, const Tpc& pc,
			      const Tpd& pd)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << 
		  destination << ' ' << pa << ' ' << pb << ' ' << pc << 
		  " :" << pd << lineTerminator;
		outputQueue.push(output.str());
	     };
	 template <class To, class Td, class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe>
	   void sendMessageTo(const To& origin, const Td& destination,
			      const Tc& command,
			      const Tpa& pa, const Tpb& pb, const Tpc& pc,
			      const Tpd& pd, const Tpe& pe)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << 
		  destination << ' ' << pa << ' ' << pb << ' ' << pc <<
		  ' ' << pd << " :" << pe << lineTerminator;
		outputQueue.push(output.str());
	     };
	 template <class To, class Td, class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe,
	           class Tpf>
	   void sendMessageTo(const To& origin, const Td& destination,
			      const Tc& command,
			      const Tpa& pa, const Tpb& pb, const Tpc& pc,
			      const Tpd& pd, const Tpe& pe, const Tpf& pf)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << 
		  destination << ' ' << pa << ' ' << pb << ' ' << pc << 
		  ' ' << pd << ' ' << pe << " :" << pf << lineTerminator;
		outputQueue.push(output.str());
	     };
	 template <class To, class Td, class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe,
	           class Tpf, class Tpg>
	   void sendMessageTo(const To& origin, const Td& destination,
			      const Tc& command,
			      const Tpa& pa, const Tpb& pb, const Tpc& pc,
			      const Tpd& pd, const Tpe& pe, const Tpf& pf,
			      const Tpg& pg)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << 
		  destination << ' ' << pa << ' ' << pb << ' ' << pc << 
		  ' ' << pd << ' ' << pe << ' ' << pf << " :" << pg <<
		  lineTerminator;
		outputQueue.push(output.str());
	     };
	 template <class To, class Td, class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe,
	           class Tpf, class Tpg, class Tph>
	   void sendMessageTo(const To& origin, const Td& destination,
			      const Tc& command,
			      const Tpa& pa, const Tpb& pb, const Tpc& pc,
			      const Tpd& pd, const Tpe& pe, const Tpf& pf,
			      const Tpg& pg, const Tph& ph)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << 
		  destination << ' ' << pa << ' ' << pb << ' ' << pc <<
		  ' ' << pd << ' ' << pe << ' ' << pf << ' ' << pg <<
		  " :" << ph << lineTerminator;
		outputQueue.push(output.str());
	     };
      };
   }; // namespace LibIRC2
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_IRC2_PROTOCOL_H_
   
