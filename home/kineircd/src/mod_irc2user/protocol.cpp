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

#include "mod_irc2user/protocol.h"

using namespace Kine::mod_irc2user;


/* Protocol - Constructor/Initialise the connection
 * Original 12/08/2001 simonb
 */
Protocol::Protocol(const Kine::Registrant& registrant,
		   Kine::Connection& connection, std::string& inputQueue,
		   std::string& outputQueue)
  : Kine::LibIRC2::Protocol(connection, inputQueue, outputQueue)
{
   // vv temporary :)
   std::cout << "irc2user is pushing stuff to the output queue" << std::endl;
   std::ostringstream output;
   std::ostringstream output2;
   
   output <<
     ':' << config().getOptionsServerName() << " 001 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 002 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 003 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 004 " << registrant.name <<
     " :?\r\n:" <<
     config().getOptionsServerName() << " 251 " << registrant.name <<
     " :?\r\n:" <<
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

   Languages::parameterList_type params;
   std::string foo("foo");
   params.push_back(&foo);
   std::string bah("bah");
   params.push_back(&bah);
   std::string baz("baz");
   params.push_back(&baz);
   
   output2 << ':' <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag  0: " << langs().get("en", 0, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag  1: " << langs().get("en", 1, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag  2: " << langs().get("en", 2, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag  3: " << langs().get("en", 3, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag  4: " << langs().get("en", 4, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag  5: " << langs().get("en", 5, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag  6: " << langs().get("en", 6, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag  7: " << langs().get("en", 7, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag  8: " << langs().get("en", 8, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag  9: " << langs().get("en", 9, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag 10: " << langs().get("en", 10, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag 11: " << langs().get("en", 11, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag 12: " << langs().get("en", 12, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag 13: " << langs().get("en", 13, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag 14: " << langs().get("en", 14, &params) << "\r\n:" <<
     config().getOptionsServerName() << " NOTICE " << registrant.name <<
     " :Tag 15: " << langs().get("en", 15, &params) << "\r\n";
   
   Protocol::outputQueue.push(output.str());
   Protocol::outputQueue.push(output2.str());
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
