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
	 // Constructor
	 explicit Protocol(Kine::Connection& c)
	   : Output(c)
	   {};

	 // Constructor (for migrating I/O queues)
	 explicit Protocol(Kine::Connection& c,
			   const std::string& iq,
			   const std::string& oq)
	   : Input(iq),
	     Output(c, oq)
	   {};
	 
       public:
	 //! Destructor
	 virtual ~Protocol(void)
	   {};
	 
	 
	 //! Send an appropriately formatted message with raw data (no TO/FROM)
	 template <class Tc, class Tp>
	   void sendRawMessage(const Tc& command, const Tp& parameters)
	   {
	      std::ostringstream output;
	      output << command << ' ' << parameters;
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
   
