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

#ifndef _INCLUDE_KINEIRCD_IRC2_PROTOCOL_H_
# define _INCLUDE_KINEIRCD_IRC2_PROTOCOL_H_ 1

# include <queue>
# include <vector>
# include <string>
# include <sstream>
# include <iomanip>
# include <aisutil/string/string.h>
# include <kineircd/libirc2/input.h>
# include <kineircd/libirc2/output.h>
# include <kineircd/user.h>
# include <kineircd/myserver.h>
# include <kineircd/libirc2/numerics.h>
# include <kineircd/libirc2/utility.h>


namespace Kine {
   namespace LibIRC2 {
      /*!
       * \brief The IRC-2 protocol suite base class
       * 
       * \ingroup LibIRC2
       */
      class Protocol : public Input, public Output {
       public:
	 typedef unsigned int messageSize_type;

	 //! A replacement parameter (literally a '*', used in many places)
	 static const char* const replacementParameter;
	 static const char replacementCharacter;

	 //! The 'ERROR' command name
	 static const char* const errorCommandName;
	 
       protected:
	 // The type of a 'parameter list'
	 typedef std::vector < AIS::Util::String > parameters_type;

       private:
	 // Message counters
	 messageCount_type sentMessageCount;
	 messageCount_type receivedMessageCount;
	 
	 // Appropriately parse a protocol message
	 virtual void parseMessage(const std::string& origin,
				   const std::string& command,
				   const parameters_type& parameters) = 0;

	 // Break up a protocol message into its components
	 virtual void parseLine(const std::string& line);
	 
       protected:
	 // Constructor
	 explicit Protocol(Kine::Connection& c)
	   : Output(c),
	     sentMessageCount(0),
	     receivedMessageCount(0)
	   {};

	 // Constructor (for migrating I/O queues)
	 explicit Protocol(Kine::Connection& c,
			   const std::string& iq,
			   const std::string& oq)
	   : Input(iq),
	     Output(c, oq),
	     sentMessageCount(0),
	     receivedMessageCount(0)
	   {};

	 
	 //! Insert a message into the output queue (raw)
	 inline void sendRawLine(const std::string& input)
	   {
	      outputLine(input);
	      ++sentMessageCount;
	   };
	 
       public:
	 //! Destructor
	 virtual ~Protocol(void)
	   {};
	 
	 
	 // Return the number of messages sent through this protocol
	 const messageCount_type getSentMessageCount(void) const
	   { return sentMessageCount; };
	 
	 // Return the number of messages received through this protocol
	 const messageCount_type getReceivedMessageCount(void) const
	   { return receivedMessageCount; };


	 //! Send an appropriately formatted message with raw data (no TO/FROM)
	 template <class Tc, class Tp>
	   void sendRawMessage(const Tc& command, const Tp& parameters)
	   {
	      std::ostringstream output;
	      output << command << ' ' << parameters;
	      sendRawLine(output.str());
	   };

	 
	 //! Send an appropriately formatted message, without TO/FROM fields
	 template <class Tc>
	   void sendMessage(const Tc& command)
	   {
	      std::ostringstream output;
	      output << command;
	      sendRawLine(output.str());
	   };
	 template <class Tc,
	           class Tpa>
	   void sendMessage(const Tc& command,
			    const Tpa& pa)
	   {
	      std::ostringstream output;
	      output << command << " :" << pa;
	      sendRawLine(output.str());
	   };
	 template <class Tc,
	           class Tpa, class Tpb>
	   void sendMessage(const Tc& command,
			    const Tpa& pa, const Tpb& pb)
	   {
	      std::ostringstream output;
	      output << command << ' ' << pa << " :" << pb;
	      sendRawLine(output.str());
	   };
	 template <class Tc,
	           class Tpa, class Tpb, class Tpc>
	   void sendMessage(const Tc& command,
			    const Tpa& pa, const Tpb& pb, const Tpc& pc)
	   {
	      std::ostringstream output;
	      output << command << ' ' << pa << ' ' << pb << " :" << pc;
	      sendRawLine(output.str());
	   };
	 template <class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd>
	   void sendMessage(const Tc& command,
			    const Tpa& pa, const Tpb& pb, const Tpc& pc,
			    const Tpd& pd)
	   {
	      std::ostringstream output;
	      output << command << ' ' << pa << ' ' << pb << ' ' << pc <<
		" :" << pd;
	      sendRawLine(output.str());
	   };
	 template <class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe>
	   void sendMessage(const Tc& command,
			    const Tpa& pa, const Tpb& pb, const Tpc& pc,
			    const Tpd& pd, const Tpe& pe)
	   {
	      std::ostringstream output;
	      output << command << ' ' << pa << ' ' << pb << ' ' << pc <<
		' ' << pd << " :" << pe;
	      sendRawLine(output.str());
	   };
	 template <class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe,
	           class Tpf>
	   void sendMessage(const Tc& command,
			    const Tpa& pa, const Tpb& pb, const Tpc& pc,
			    const Tpd& pd, const Tpe& pe, const Tpf& pf)
	   {
	      std::ostringstream output;
	      output << command << ' ' << pa << ' ' << pb << ' ' << pc <<
		' ' << pd << ' ' << pe << " :" << pf;
	      sendRawLine(output.str());
	   };
	 template <class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe,
	           class Tpf, class Tpg>
	   void sendMessage(const Tc& command,
			    const Tpa& pa, const Tpb& pb, const Tpc& pc,
			    const Tpd& pd, const Tpe& pe, const Tpf& pf,
			    const Tpg& pg)
	   {
	      std::ostringstream output;
	      output << command << ' ' << pa << ' ' << pb << ' ' << pc <<
		' ' << pd << ' ' << pe << ' ' << pf << " :" << pg;
	      sendRawLine(output.str());
	   };
	 template <class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe,
	           class Tpf, class Tpg, class Tph>
	   void sendMessage(const Tc& command,
			    const Tpa& pa, const Tpb& pb, const Tpc& pc,
			    const Tpd& pd, const Tpe& pe, const Tpf& pf,
			    const Tpg& pg, const Tph& ph)
	   {
	      std::ostringstream output;
	      output << command << ' ' << pa << ' ' << pb << ' ' << pc <<
		' ' << pd << ' ' << pe << ' ' << pf << ' ' << pg <<
		" :" << ph;
	      sendRawLine(output.str());
	   };
	 template <class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe,
	           class Tpf, class Tpg, class Tph, class Tpi>
	   void sendMessage(const Tc& command,
			    const Tpa& pa, const Tpb& pb, const Tpc& pc,
			    const Tpd& pd, const Tpe& pe, const Tpf& pf,
			    const Tpg& pg, const Tph& ph, const Tpi& pi)
	   {
	      std::ostringstream output;
	      output << command << ' ' << pa << ' ' << pb << ' ' << pc <<
		' ' << pd << ' ' << pe << ' ' << pf << ' ' << pg <<
		' ' << ph << " :" << pi;
	      sendRawLine(output.str());
	   };
	 
	 
	 //! Send a properly formatted message with FROM field
	 template <class To, class Tc>
	   void sendMessageFrom(const To& origin, const Tc& command)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command;
		sendRawLine(output.str());
	     };
	 template <class To, class Tc,
	           class Tpa>
	   void sendMessageFrom(const To& origin, const Tc& command,
				const Tpa& pa)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << " :" << pa;
		sendRawLine(output.str());
	     };
	 template <class To, class Tc,
	           class Tpa, class Tpb>
	   void sendMessageFrom(const To& origin, const Tc& command,
				const Tpa& pa, const Tpb& pb)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << pa <<
		  " :" << pb;
		sendRawLine(output.str());
	     };
	 template <class To, class Tc,
	           class Tpa, class Tpb, class Tpc>
	   void sendMessageFrom(const To& origin, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << pa <<
		  ' ' << pb << " :" << pc;
		sendRawLine(output.str());
	     };
	 template <class To, class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd>
	   void sendMessageFrom(const To& origin, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc,
				const Tpd& pd)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << pa <<
		  ' ' << pb << ' ' << pc << " :" << pd;
		sendRawLine(output.str());
	     };
	 template <class To, class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe>
	   void sendMessageFrom(const To& origin, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc,
				const Tpd& pd, const Tpe& pe)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << pa <<
		  ' ' << pb << ' ' << pc << ' ' << pd << " :" << pe;
		sendRawLine(output.str());
	     };
	 template <class To, class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe,
	           class Tpf>
	   void sendMessageFrom(const To& origin, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc,
				const Tpd& pd, const Tpe& pe, const Tpf& pf)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << pa <<
		  ' ' << pb << ' ' << pc << ' ' << pd << ' ' << pe <<
		  " :" << pf;
		sendRawLine(output.str());
	     };
	 template <class To, class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe,
	           class Tpf, class Tpg>
	   void sendMessageFrom(const To& origin, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc,
				const Tpd& pd, const Tpe& pe, const Tpf& pf,
				const Tpg& pg)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << pa <<
		  ' ' << pb << ' ' << pc << ' ' << pd << ' ' << pe << 
		  ' ' << pf << " :" << pg;
		sendRawLine(output.str());
	     };
	 template <class To, class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe,
	           class Tpf, class Tpg, class Tph>
	   void sendMessageFrom(const To& origin, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc,
				const Tpd& pd, const Tpe& pe, const Tpf& pf,
				const Tpg& pg, const Tph& ph)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << pa <<
		  ' ' << pb << ' ' << pc << ' ' << pd << ' ' << pe <<
		  ' ' << pf << ' ' << pg << " :" << ph;
		sendRawLine(output.str());
	     };
	 template <class To, class Tc,
	           class Tpa, class Tpb, class Tpc, class Tpd, class Tpe,
	           class Tpf, class Tpg, class Tph, class Tpi>
	   void sendMessageFrom(const To& origin, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc,
				const Tpd& pd, const Tpe& pe, const Tpf& pf,
				const Tpg& pg, const Tph& ph, const Tpi& pi)
	     {
		std::ostringstream output;
		output << ':' << origin << ' ' << command << ' ' << pa <<
		  ' ' << pb << ' ' << pc << ' ' << pd << ' ' << pe <<
		  ' ' << pf << ' ' << pg << ' ' << ph << " :" << pi;
		sendRawLine(output.str());
	     };

	 
	 //! Send a message from a particular client
	 template <class Tc>
	   void sendMessageFrom(const Client& client, const Tc& command)
	     {
		sendMessageFrom(delocaliseStr(Utility::makeNickUserHostIdent(client)),
				command);
	     };
	 template <class Tc, class Tpa>
	   void sendMessageFrom(const Client& client, const Tc& command,
				const Tpa& pa)
	     {
		sendMessageFrom(delocaliseStr(Utility::makeNickUserHostIdent(client)),
				command, pa);
	     };
	 template <class Tc, class Tpa, class Tpb>
	   void sendMessageFrom(const Client& client, const Tc& command,
				const Tpa& pa, const Tpb& pb)
	     {
		sendMessageFrom(delocaliseStr(Utility::makeNickUserHostIdent(client)),
				command, pa, pb);
	     };
	 template <class Tc, class Tpa, class Tpb, class Tpc>
	   void sendMessageFrom(const Client& client, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc)
	     {
		sendMessageFrom(delocaliseStr(Utility::makeNickUserHostIdent(client)),
				command, pa, pb, pc);
	     };
	 template <class Tc, class Tpa, class Tpb, class Tpc, class Tpd>
	   void sendMessageFrom(const Client& client, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc,
				const Tpd& pd)
	     {
		sendMessageFrom(delocaliseStr(Utility::makeNickUserHostIdent(client)),
				command, pa, pb, pc, pd);
	     };
	 template <class Tc, class Tpa, class Tpb, class Tpc, class Tpd,
	           class Tpe>
	   void sendMessageFrom(const Client& client, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc,
				const Tpd& pd, const Tpe& pe)
	     {
		sendMessageFrom(delocaliseStr(Utility::makeNickUserHostIdent(client)),
				command, pa, pb, pc, pd, pe);
	     };
	 template <class Tc, class Tpa, class Tpb, class Tpc, class Tpd,
	           class Tpe, class Tpf>
	   void sendMessageFrom(const Client& client, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc,
				const Tpd& pd, const Tpe& pe, const Tpf& pf)
	     {
		sendMessageFrom(delocaliseStr(Utility::makeNickUserHostIdent(client)),
				command, pa, pb, pc, pd, pe, pf);
	     };
	 template <class Tc, class Tpa, class Tpb, class Tpc, class Tpd,
	           class Tpe, class Tpf, class Tpg>
	   void sendMessageFrom(const Client& client, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc,
				const Tpd& pd, const Tpe& pe, const Tpf& pf,
				const Tpg& pg)
	     {
		sendMessageFrom(delocaliseStr(Utility::makeNickUserHostIdent(client)),
				command, pa, pb, pc, pd, pe, pf, pg);
	     };
	 template <class Tc, class Tpa, class Tpb, class Tpc, class Tpd,
	           class Tpe, class Tpf, class Tpg, class Tph>
	   void sendMessageFrom(const Client& client, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc,
				const Tpd& pd, const Tpe& pe, const Tpf& pf,
				const Tpg& pg, const Tph& ph)
	     {
		sendMessageFrom(delocaliseStr(Utility::makeNickUserHostIdent(client)),
				command, pa, pb, pc, pd, pe, pf, pg, ph);
	     };
	 template <class Tc, class Tpa, class Tpb, class Tpc, class Tpd,
	           class Tpe, class Tpf, class Tpg, class Tph, class Tpi>
	   void sendMessageFrom(const Client& client, const Tc& command,
				const Tpa& pa, const Tpb& pb, const Tpc& pc,
				const Tpd& pd, const Tpe& pe, const Tpf& pf,
				const Tpg& pg, const Tph& ph, const Tpi& pi)
	     {
		sendMessageFrom(delocaliseStr(Utility::makeNickUserHostIdent(client)),
				command, pa, pb, pc, pd, pe, pf, pg, ph, pi);
	     };

	 
	 //! Send a numeric
	 void sendNumeric(const Client& destination,
			  const Numerics::numeric_type numeric)
	   {
	      std::ostringstream output;
	      output << ':' << delocaliseStr(myServer().getName()) << 
		' ' << std::setfill('0') << std::setw(3) << numeric << 
		' ' << delocaliseStr(destination.getNickname());
	      sendRawLine(output.str());
	   };
	 template <class Ta>
	   void sendNumeric(const Client& destination,
			    const Numerics::numeric_type numeric,
			    const Ta& pa)
	     {
		std::ostringstream output;
		output << ':' << delocaliseStr(myServer().getName()) << 
		  ' ' << std::setfill('0') << std::setw(3) << numeric << 
		  ' ' << delocaliseStr(destination.getNickname()) << 
		  " :" << pa;
		sendRawLine(output.str());
	     };
	 template <class Ta, class Tb>
	   void sendNumeric(const Client& destination,
			    const Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb)
	     {
		std::ostringstream output;
		output << ':' << delocaliseStr(myServer().getName()) << 
		  ' ' << std::setfill('0') << std::setw(3) << numeric <<
		  ' ' << delocaliseStr(destination.getNickname()) <<
		  ' ' << pa << " :" << pb;
		sendRawLine(output.str());
	     };
	 template <class Ta, class Tb, class Tc>
	   void sendNumeric(const Client& destination,
			    const Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc)
	     {
		std::ostringstream output;
		output << ':' << delocaliseStr(myServer().getName()) <<
		  ' ' << std::setfill('0') << std::setw(3) << numeric <<
		  ' ' << delocaliseStr(destination.getNickname()) <<
		  ' ' << pa << ' ' << pb << " :" << pc;
		sendRawLine(output.str());
	     };
	 template <class Ta, class Tb, class Tc, class Td>
	   void sendNumeric(const Client& destination,
			    const Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd)
	     {
		std::ostringstream output;
		output << ':' << delocaliseStr(myServer().getName()) <<
		  ' ' << std::setfill('0') << std::setw(3) << numeric <<
		  ' ' << delocaliseStr(destination.getNickname()) <<
		  ' ' << pa << ' ' << pb << ' ' << pc << " :" << pd;
		sendRawLine(output.str());
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te>
	   void sendNumeric(const Client& destination,
			    const Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe)
	     {
		std::ostringstream output;
		output << ':' << delocaliseStr(myServer().getName()) <<
		  ' ' << std::setfill('0') << std::setw(3) << numeric <<
		  ' ' << delocaliseStr(destination.getNickname()) <<
		  ' ' << pa << ' ' << pb << ' ' << pc << ' ' << pd <<
		  " :" << pe;
		sendRawLine(output.str());
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te, class Tf>
	   void sendNumeric(const Client& destination,
			    const Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe, const Tf& pf)
	     {
		std::ostringstream output;
		output << ':' << delocaliseStr(myServer().getName()) <<
		  ' ' << std::setfill('0') << std::setw(3) << numeric <<
		  ' ' << delocaliseStr(destination.getNickname()) <<
		  ' ' << pa << ' ' << pb << ' ' << pc << ' ' << pd <<
		  ' ' << pe << " :" << pf;
		sendRawLine(output.str());
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te, class Tf,
	           class Tg>
	   void sendNumeric(const Client& destination,
			    const Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe, const Tf& pf,
			    const Tg& pg)
	     {
		std::ostringstream output;
		output << ':' << delocaliseStr(myServer().getName()) << 
		  ' ' << std::setfill('0') << std::setw(3) << numeric << 
		  ' ' << delocaliseStr(destination.getNickname()) <<
		  ' ' << pa << ' ' << pb << ' ' << pc << ' ' << pd <<
		  ' ' << pe << ' ' << pf << " :" << pg;
		sendRawLine(output.str());
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te, class Tf,
	           class Tg, class Th>
	   void sendNumeric(const Client& destination,
			    const Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe, const Tf& pf,
			    const Tg& pg, const Th& ph)
	     {
		std::ostringstream output;
		output << ':' << delocaliseStr(myServer().getName()) <<
		  ' ' << std::setfill('0') << std::setw(3) << numeric <<
		  ' ' << delocaliseStr(destination.getNickname()) <<
		  ' ' << pa << ' ' << pb << ' ' << pc << ' ' << pd <<
		  ' ' << pe << ' ' << pf << ' ' << pg << " :" << ph;
		sendRawLine(output.str());
	     };
	 template <class Ta, class Tb, class Tc, class Td, class Te, class Tf,
	           class Tg, class Th, class Ti>
	   void sendNumeric(const Client& destination,
			    const Numerics::numeric_type numeric,
			    const Ta& pa, const Tb& pb, const Tc& pc,
			    const Td& pd, const Te& pe, const Tf& pf,
			    const Tg& pg, const Th& ph, const Ti& pi)
	     {
		std::ostringstream output;
		output << ':' << delocaliseStr(myServer().getName()) <<
		  ' ' << std::setfill('0') << std::setw(3) << numeric <<
		  ' ' << delocaliseStr(destination.getNickname()) <<
		  ' ' << pa << ' ' << pb << ' ' << pc << ' ' << pd <<
		  ' ' << pe << ' ' << pf << ' ' << pg << ' ' << ph <<
		  " :" << pi;
		sendRawLine(output.str());
	     };


	 //! Send an error message and disconnect
	 template <class Tp>
	   void sendError(const Tp& error)
	     {
		sendMessage(errorCommandName, error);
		connection.goodbye();
	     };


	 //! Send RPL_TIMEONSERVERIS to the given client
	 void sendTimeOnServer(const User& user);
	 
	 
	 //! Handle the various commands which are quite common in IRC-2 modules
	 void doADMIN(const User& user);
	 void doINFO(const User& user);
	 void doLUSERS(const User& user);
	 void doMOTD(const User& user, const bool justConnected = false);
	 void doSTATS(const User& user, const std::string& request);
	 void doSUMMON(const User& user, const std::string& username);
	 void doTIME(const User& user);
	 void doUSERS(const User& user);
	 void doVERSION(const User& user);
	 void doWHOIS(const User& user, const std::string& targets);
	 void doWHOWAS(const User& user, const std::string& targets,
		       int count = -1);
      };
   }; // namespace LibIRC2
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_IRC2_PROTOCOL_H_
   
