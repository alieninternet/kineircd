/* $Id$
 * The IRC-2 user protocol class
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <iostream>
#include <kineircd/config.h>
#include <kineircd/languages.h>
#include <kineircd/version.h>

#include "mod_irc2user/protocol.h"
#include "mod_irc2user/language.h"

using namespace Kine::mod_irc2user;


/* Protocol - Constructor/Initialise the connection
 * Original 12/08/2001 simonb
 */
Protocol::Protocol(const Kine::Registrant& registrant,
		   Kine::Connection& connection, std::string& inputQueue,
		   std::string& outputQueue)
  : Kine::LibIRC2::Protocol(connection, inputQueue, outputQueue),
    user(registrant)
{
   // vv temporary :)
   std::ostringstream output;
   
   output <<
     ':' << config().getOptionsServerName() << " 001 " << registrant.name <<
     " :" << GETLANG(irc2_RPL_WELCOME, 
		     "nick", "user", "host.name") << "\r\n:" <<
     config().getOptionsServerName() << " 002 " << registrant.name <<
     " :" << GETLANG(irc2_RPL_YOURHOST,
		     config().getOptionsServerName(),
		     Kine::Version::version) << "\r\n:" <<
     config().getOptionsServerName() << " 003 " << registrant.name <<
     " :" << GETLANG(irc2_RPL_CREATED,
		     Kine::Version::buildTime) << "\r\n:" <<
     config().getOptionsServerName() << " 004 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 251 " << registrant.name <<
     " :♥ = ☮ & ☯\r\n:" <<
     config().getOptionsServerName() << " 252 0 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 253 0 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 254 0 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 255 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 375 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 376 " << registrant.name <<
     " :?\r\n";

   Protocol::outputQueue.push(output.str());
   // ^^ temporary :)
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
     destination << std::endl;
}
