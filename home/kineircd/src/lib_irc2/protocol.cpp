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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <aisutil/string/tokens.h>
#include <kineircd/config.h>

#include "kineircd/irc2/protocol.h"

using AISutil::StringTokens;
using namespace Kine::LibIRC2;


// Replacement characters
const char* const Protocol::replacementParameter = 
  "*";
const char Protocol::replacementCharacter =
  '*';


// Error command name (put here so there aren't *heaps* per template use)
const char* const Protocol::errorCommandName =
  "ERROR";


/* parseLine - Break up a protocol message into its components, and pass it on
 * Original 12/08/2001 simonb
 * Note: This is shockingly inefficient
 */
void Protocol::parseLine(const std::string& line)
{
   StringTokens message(line);
   std::string origin;
   std::string command;
   parameters_type parameters;

   // Oh look! It's a message! Increase the received message counter
   ++receivedMessageCount;
   
   // Do we have an origin/destination to break out?
   if (line[0] == ':') {
      origin = message.nextToken().substr(1);
      command = message.nextToken().toUpper();
   } else {
      command = message.nextToken().toUpper();
   }
   
   // Grab the parameters...
   std::string thingy;
   while (message.hasMoreTokens()) {
      thingy = message.nextToken();
      
      // Last one?
      if (thingy[0] == ':') {
	 if (message.hasMoreTokens()) {
	    thingy = thingy.substr(1) + ' ' + message.rest();
	 } else {
	    thingy = thingy.substr(1);
	 }
	 parameters.push_back(thingy);
	 break;
      }
      
      parameters.push_back(thingy);
   }
   
   // Check - we must at least have a command to pass
   if (!command.empty()) {
      // Hand the line over to the message handler
      parseMessage(origin, command, parameters);
   }
}
