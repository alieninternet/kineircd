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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <aisutil/string/tokens.h>
#include <kineircd/config.h>
#include <kineircd/version.h>

#include "libkineircd_irc2/protocol.h"

using AISutil::StringTokens;
using namespace Kine::LibIRC2;


/* Protocol - Constructor for migration between protocols (copies I/O queues)
 * Original 12/08/2001 simonb
 * Note: This could be more efficient :(
 */
Protocol::Protocol(Kine::Connection& c, std::string& iq, std::string& oq)
  : Kine::Protocol(c),
    lineTerminator("\r\n"),
    inputQueue(iq)
{
   // Push the old output queue onto the new one..
   outputQueue.push(oq);
}


/* parseLine - Break up a protocol message into its components, and pass it on
 * Original 12/08/2001 simonb
 * Note: This could be more efficient :(
 */
void Protocol::parseLine(const std::string& line)
{
   StringTokens message(line);
   std::string origin;
   std::string command;
   std::string destination;
   parameters_type parameters;

   // Do we have an origin/destination to break out?
   if (line[0] == ':') {
      origin = message.nextToken().substr(1);
      command = message.nextToken().toUpper();
      destination = message.nextToken();
   } else {
      command = message.nextToken().toUpper();
   }
   
   // Grab the parameters...
   std::string thingy;
   while (message.hasMoreTokens()) {
      thingy = message.nextToken();
      
      // Last one?
      if (thingy[0] == ':') {
	 thingy = thingy.substr(1) + ' ' + message.rest();
	 parameters.push_back(thingy);
	 break;
      }
      
      parameters.push_back(thingy);
   }
   
   // Check - we must at least have a command to pass
   if (!command.empty()) {
      // Hand the line over to the message handler
      parseMessage(origin, destination, command, parameters);
   }
}


/* handleInput - Handle incoming data
 * Original 11/08/2001 simonb
 * Note: This could be more efficient :(
 */
void Protocol::handleInput(std::stringstream& data)
{
   for (;;) {
      // Make sure the stream has something left..
      if (data.peek() == -1) {
	 return;
      }

      // Check for special chars..
      if (data.peek() == '\0') {
	// Quietly ignore it and move along..
	(void)data.ignore();
      } else if ((data.peek() == '\r') || (data.peek() == '\n')) {
	 // Skip it - we are at the end of a line
	 (void)data.ignore();
	 
	 // If the next char is also a part of it (ie. a \r\n sequence) skip it
	 if ((data.peek() == '\r') || (data.peek() == '\n')) {
	    (void)data.ignore();
	 }

	 // Check if the buffer has something in it (perhaps a message?)
	 if (!inputQueue.empty()) {
	    // Hand the data over to the parser (as a single line
	    parseLine(inputQueue);
	    
	    // Clear the buffer
	    inputQueue.clear();
	 }
      } else {
	 /* If the buffer has grown too large. For strict compatibility, we
	  * must refer to RFC1459 which states the maximum message length
	  * must be 512 octets or smaller, including the \r\n termination.
	  * We don't look for the \r\n termination, we look for \r\n, \r,
	  * or \n. Since this is the case, we make sure lines are under 512
	  * octets, or rather 511 octets is our threashold. To avoid people
	  * pumping inordinate amount of data our way, we will disconnect them
	  * should they break this limit.
	  */
	 if (inputQueue.length() > 510) {
	    connection.goodbye();
	 }
	 
	 // Just add the char to the buffer
	 inputQueue += (char)data.get();
      }
   }
}


/* withdrawOutput - Remove up to the amount given of octets from the queue
 * Original 28/09/2002 simonb
 * Note: This could be more efficient :(
 */
std::string Protocol::withdrawOutput(AISutil::Socket::blockSize_type amount)
{
   std::string output;
   
   /* Append as much data as we can to the output without breaking the amount
    * limit we were given
    */
   while (!outputQueue.empty() && (outputQueue.front().size() <= amount)) {
      output += outputQueue.front();
      amount -= outputQueue.front().size();
      outputQueue.pop();
   }
   
   // Is there anything left we might also be able to send?
   if (!outputQueue.empty() && (amount > 0)) {
      amount--;
      output += outputQueue.front().substr(0, amount);
      outputQueue.front().erase(0, amount);
   }
   
   // Return the output
   return output;
}


/* sendISUPPORT - Send ISUPPORT information to the given user..
 * Original 24/08/2001 simonb
 */
void Protocol::sendISUPPORT(const Kine::User& user)
{
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_ISUPPORT,
		 "isupport");
}


/* sendLUSERS - Send an LUSERS reply to the given user..
 * Original 13/08/2001 simonb
 */
void Protocol::sendLUSERS(const Kine::User& user)
{
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_LUSERCLIENT,
		 "Stuff..");
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_LUSEROP,
		 0,
		 "luserop");
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_LUSERSTAFF,
		 0,
		 "luserstaff");
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_LUSERUNKNOWN,
		 0,
		 "luserunknown");
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_LUSERCHANNELS,
		 0,
		 0,
		 "luserchannels");
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_LUSERME,
		 "luserme");
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_LOCALUSERS,
		 "localusers");
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_GLOBALUSERS,
		 "globalusers");
}


/* sendMOTD - Send our MOTD to the given user..
 * Original 13/08/2001 simonb
 */
void Protocol::sendMOTD(const Kine::User& user,
			const bool justConnected)
{
   // Send the MOTD header
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_MOTDSTART,
		 "start motd");

   // Send this line
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_MOTD,
		 "- This is MOTD data");
   
   // Send the MOTD footer
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_ENDOFMOTD,
		 "end motd");
}


/* sendVERSION - Send a VERSION reply to the given user..
 * Original 24/08/2001 simonb
 */
void Protocol::sendVERSION(const Kine::User& user)
{
   // Send the RPL_VERSION reply
   sendMessageTo(config().getOptionsServerName(), user.getNickname(),
		 Numerics::RPL_VERSION,
		 config().getOptionsServerName(),
		 Version::version,
		 Version::versionChars);
   
   // Also send the RPL_ISUPPORT stuff
   sendISUPPORT(user);
}
