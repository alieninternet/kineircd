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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <iostream>
#include <sstream>
#include <kineircd/config.h>
#include <kineircd/languages.h>
#include <kineircd/version.h>

#include "mod_irc2user/protocol.h"
#include "mod_irc2user/language.h"
#include "mod_irc2user/commands.h"

using namespace Kine::mod_irc2user;


/* Protocol - Constructor/Initialise the connection
 * Original 12/08/2001 simonb
 */
Protocol::Protocol(const Kine::Registrant& registrant,
		   Kine::Connection& connection, std::string& inputQueue,
		   std::string& outputQueue)
  : Kine::LibIRC2::Protocol(connection, inputQueue, outputQueue),
    user(registrant, connection.getSocket().getRemoteAddress())
{
   // vv temporary :)
   std::ostringstream output;

   // Welcome the user to the server (001)
   sendNumeric(LibIRC2::Numerics::RPL_WELCOME,
	       GETLANG(irc2_RPL_WELCOME,
		       user.getNickname(),
		       user.getUsername(),
		       user.getHostname()));
   
   // Tell the user about ourselves - hostname and version (002)
   sendNumeric(LibIRC2::Numerics::RPL_YOURHOST,
	       GETLANG(irc2_RPL_YOURHOST,
		       config().getOptionsServerName(),
		       Kine::Version::versionFull));
   
   // Tell the user when the server was created. Why?? (003)
   sendNumeric(LibIRC2::Numerics::RPL_CREATED,
	       GETLANG(irc2_RPL_CREATED,
		       Kine::Version::buildTime));
   
   // Tell the user a little bit about what we know.. (004)
   sendNumeric(LibIRC2::Numerics::RPL_MYINFO,
	       config().getOptionsServerName(),
	       Kine::Version::version,
	       "umodes",
	       "cmodes",
	       "cparammodes",
	       "uparammodes",
	       "smodes",
	       "sparammodes");
   
   // Tell the user a about what we can do.. (005)
   sendNumeric(LibIRC2::Numerics::RPL_ISUPPORT,
	       /* stuff */
	       GETLANG(irc2_RPL_ISUPPORT));
     
   // Send the LUSER output, for some reason some clients want this??
   output << ':' <<
     config().getOptionsServerName() << " 251 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 252 0 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 253 0 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 254 0 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 255 " << registrant.name <<
     " :?\r\n";
   
   // Send an MOTD, of some sort..
   output << ':' <<
     config().getOptionsServerName() << " 375 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 376 " << registrant.name <<
     " :?\r\n";

   /* THIS IS TEMPORARY!! */ Protocol::outputQueue.push(output.str());
}


/* parseMessage - Appropriately parse a protocol message
 * Original 12/08/2001 simonb
 */
void Protocol::parseMessage(const std::string& origin,
			    const std::string& destination,
			    const std::string& command,
			    const Kine::LibIRC2::Protocol::parameters_type&
			    parameters)
{
   std::cout << "COMMAND: " << command << "\nFrom: " << origin << "\nTo: " <<
     destination << "\nParam count: " << parameters.size() << std::endl;
   
   // Try and find the command..
   const Commands::CommandInfo* const commandInfo =
     Commands::getInstance().findCommand(command);
   
   // Did we find it? (most of the time we will find it
   if (commandInfo != 0) {
      // Do we have enough parameters?
      if (parameters.size() >= commandInfo->minimumParams) {
	 
	 // We're done!
	 return;
      }
      
      // Complain about not having enough parameters, and return
      sendNumeric(LibIRC2::Numerics::ERR_NEEDMOREPARAMS,
		  GETLANG(irc2_ERR_NEEDMOREPARAMS));
      return;
   }

   // If we got here, the command was not found - tell the user the bad news
   sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNCOMMAND,
	       command, GETLANG(irc2_ERR_UNKNOWNCOMMAND));
}
