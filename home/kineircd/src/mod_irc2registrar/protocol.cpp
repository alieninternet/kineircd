/* $Id$
 * Handle registration data (before handling a real handler)
 * 
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

#include "autoconf.h"
#include "kineircd/kineircdconf.h"

#include <sstream>
#include <iomanip>

#include "kineircd/protocolinfo.h"
#include "kineircd/protocolname.h"
#include "registrar.h"
#include "regnumerics.h"
#include "debug.h"

using namespace Kine;
using AISutil::String;
using AISutil::StringTokens;


// Master command table (zero terminated, ordered alphabetically)
const Registrar::commandTable_type Registrar::commandTable[] = {
     { "CAPAB",		&Registrar::parseCAPAB },
     { "IIRCN",		&Registrar::parseIIRCN },
     { "NICK",		&Registrar::parseNICK },
     { "PASS",		&Registrar::parsePASS },
     { "PONG",		&Registrar::parsePONG },
     { "QUIT",		&Registrar::parseQUIT },
     { "SERVER",	&Registrar::parseSERVER },
     { "SERVICE",	&Registrar::parseSERVICE },
     { "USER",		&Registrar::parseUSER },
     { 0, 0 }
};


/* sendNumeric - Send a numeric (no payload)
 * Original 12/08/2001 simonb
 */
void Registrar::sendNumeric(const RegistrationNumerics::numeric_type numeric)
{
   std::ostringstream output;
   
   /* Output the server name and the numeric (note, we do not pre-pad the
    * numeric because all registration numerics are over 100 anyway!
    */
   output <<
     ':' << connection.getDaemon().getConfig().getOptionsServerName() <<
     ' ' << (int)numeric;
   
   // Determine the appropriate nickname.
   if (registrantData.nickname.empty()) {
      output << " *";
   } else {
      output << ' ' << registrantData.nickname;
   }

   /* Terminate the line according to RFC1459; The colon is to avoid any
    * potential problems with really brain-dead clients...
    */
   output << " :\r\n";
   
   // Throw the line onto the output queue
   outputQueue.push(output.str());
}


/* sendNumeric - Send a numeric (including payload)
 * Original 12/08/2001 simonb
 */
void Registrar::sendNumeric(const RegistrationNumerics::numeric_type numeric,
			    const char* const data)
{
   std::ostringstream output;
   
   /* Output the server name and the numeric (note, we do not pre-pad the
    * numeric because all registration numerics are over 100 anyway!
    */
   output << 
     ':' << connection.getDaemon().getConfig().getOptionsServerName() <<
     ' ' << (int)numeric;
   
   // Determine the appropriate nickname.
   if (registrantData.nickname.empty()) {
      output << " * ";
   } else {
      output << ' ' << registrantData.nickname << ' ';
   }

   /* Output the payload and terminate the line according to RFC1459; The 
    * colon is to avoid any potential problems with really brain-dead 
    * clients...
    */
   output << data << " :\r\n";
   
   // Throw the line onto the output queue
   outputQueue.push(output.str());
}


/* sendError - Send an error and disconnect
 * Original 12/08/2001 simonb
 */
void Registrar::sendError(const char* const error)
{
   std::ostringstream output;
   
   // Assemble the line, nicely terminated..
   output << "ERROR :" << error << "\r\n";
   
   // Throw the line onto the output queue
   outputQueue.push(output.str());
   
   // Disconnect..
   connection.goodbye();
}


/* sendPing - Send a ping with some unpredictable data
 * Original 12/08/2001 simonb
 */
void Registrar::sendPing(void)
{
   // Make up some dodgey random stuff
//   handler->pingpong = 
//     String::printf("%08lX",
//		    ((unsigned long)Daemon::getTime() ^
//		     (unsigned long)(((0xFFFFFFFE + 1.0) * rand()) / 
//				     RAND_MAX)));

   // Set the expectant pong result string appropriately
   pongMatch = 
     Utils::baseXStr((unsigned long)(((0xFFFFFFFE + 1.0) * rand()) / RAND_MAX),
		     36);

   // Output the ping
   outputQueue.push("PING :" + pongMatch + "\r\n");
}


/* parseLine - Parse an incoming line
 * Original 12/08/2001 simonb
 */
void Registrar::parseLine(const String& line)
{
   bool found = false;

   // Tokenise the line, and grab the command
   StringTokens st(line);
   String command = st.nextToken().toUpper();

   // Run through the list and find a function..
   for (int i = 0; commandTable[i].command != 0; i++) {
      if (command == commandTable[i].command) {
	 found = true;
	 (this->*(commandTable[i].handler))(st);
	 break;
      }
   }

   ProtocolInfo* protocolInfo = 0;
   
   /* That command could have been the completion of registration.. The first
    * sign of that being true would be the registration type would have been
    * set. Let's check...
    */
   switch (registrationType) {
    case RegistrationType::NONE:
      // There is nothing to do - registration has not been completed yet.
      return;
       
    case RegistrationType::CLIENT:
      // Make sure all required fields have been given..
      if (registrantData.nickname.empty() ||
	  registrantData.username.empty() ||
	  registrantData.hostname.empty() ||
	  (pongsLeft > 0)) {
	 return;
      }
      
      // No support yet...
      sendError("Requested client protocol unavailable");
      return;
      
    case RegistrationType::IIRCN:
      // Make sure all required fields have been given..
      if (registrantData.password.empty() ||
	  registrantData.nickname.empty() ||
	  registrantData.hostname.empty() ||
	  registrantData.protocol.empty() ||
	  registrantData.linkStamp <= 0) {
	 return;
      }
      
      /* Since we know IIRCN protocols always have a protocol name, finding
       * the appropriate protocol is easy.
       */
      if ((protocolInfo = 
	   connection.getDaemon().
	   findProtocol(ProtocolName::Type::NETWORK,
			registrantData.protocol.toUpper())) == 0) {
	 // Complain
	 sendError("Requested IIRC protocol unavailable");
	 return;
      }

      break;
      
    case RegistrationType::SERVER:
      // Make sure all required fields have been given..
      if (registrantData.password.empty() ||
	  registrantData.hostname.empty()) {
	 return;
      }
      
      // No support yet...
      sendError("Requested server protocol unavailable");
      return;
      
    case RegistrationType::SERVICE:
      // Make sure all required fields have been given..
      if (registrantData.password.empty() ||
	  registrantData.nickname.empty() ||
	  registrantData.distribution.empty()) {
	 return;
      }
       
      // No support yet...
      sendError("Requested service protocol unavailable");
      return;
   }
   
   // If we got here, we are likely to switch to the newly created protocol.
#ifdef KINE_DEBUG_ASSERT
   assert(protocolInfo != 0);
#endif
    
   // First, we need to assemble a string representing the old output queue
   std::string output;
   while (!outputQueue.empty()) {
      output += outputQueue.front();
      outputQueue.pop();
   }
   
   // Second, we create the new protocol
   Protocol* const newProtocol =
     protocolInfo->createProtocol(registrantData, connection, listener,
				  buffer, output);
      
   // That protocol instance may not have been created, so let's just check..
   if (newProtocol != 0) {
      // Do the protocol hand-over
      connection.setProtocol(*newProtocol);
      
      // Delete ourself..
      delete this;
      
      // Bye bye!
      return;
   }

   // Uh oh - the protocol was not created..
   sendError("Requested protocol did not initialise");
}


/* parseCAPAB - Requested capabilities
 * Original 16/08/2001 simonb
 * Note: Capability formatting depends on the protocol usually, so we
 *       don't deal with it here but rather just give it to the protocol
 *       handler upon initialisation for it to grok.
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parseCAPAB)
{
   // Make sure we were given at least one parameter
   if (line.countTokens() < 2) {
      sendNumeric(RegistrationNumerics::ERR_NEEDMOREPARAMS, "CAPAB");
      return;
   }

   // Append the capability to the capabilities vector
   registrantData.capabilities.push_back(line.rest());

#ifdef KINE_DEBUG_PSYCHO
   std::ostringstream out;
   out << " -=>   Capability: " << registrantData.capabilities.back();
   debug(out.str());
#endif
}


/* parseIIRCN
 * Original 05/08/2002 simonb
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parseIIRCN)
{
   // Does the port this person connected to accept this type of connection?
   if (!listener.isFlagSet(Listener::Flags::ALLOW_NETWORKS)) {
      // Throw away this connection.. We should report an error here?
      connection.goodbye();
      return;
   }
   
   // Make sure this connection has not already been given a registration mode
   if (registrationType != RegistrationType::NONE) {
      sendNumeric(RegistrationNumerics::ERR_ALREADYREGISTERED);
      connection.goodbye();
      return;
   }

   // Check there are enough tokens. We consider the description field optional
   if (line.countTokens() < 6) {
      sendNumeric(RegistrationNumerics::ERR_NEEDMOREPARAMS, "IIRCN");
      return;
   }
   
   // Take out the remote network's name from the line..
   registrantData.nickname = line.nextToken();
   
   // Check that the 'from network' field is * - it must be during registration
   if (line.nextToken() != "*") {
#ifdef KINE_DEBUG_PSYCHO
      debug("Registrar::parseIIRCN() - Invalid: from network != \"*\"");
#endif
      connection.goodbye();
      return;
   }
   
   // Continue ripping out the data
   registrantData.hostname = line.nextToken();
   registrantData.protocol = line.nextToken().toUpper();
   registrantData.linkStamp = line.nextToken().toLong();
   
#ifdef KINE_DEBUG_PSYCHO
   // Output debugging info
   debug(" -=>      Network: " + registrantData.nickname);
   debug(" -=> Gateway Host: " + registrantData.hostname);
   debug(" -=>   IIRC Proto: " + registrantData.protocol);
   debug(" -->   Time stamp: " + String::convert(registrantData.linkStamp));
#endif
   
   // Check the linkstamp, it must be greater than 0..
   if (registrantData.linkStamp <= 0) {
#ifdef KINE_DEBUG_PSYCHO
      debug("Registrar::parseIIRCN() - Invalid: timestamp <= 0");
#endif
      connection.goodbye();
      return;
   }
   
   if (line.hasMoreTokens()) {
      registrantData.realname = line.nextColonToken();
#ifdef KINE_DEBUG_PSYCHO
      debug(" -=>  Description: " + registrantData.realname);
#endif
   }
   
   // Set the registration mode
   registrationType = RegistrationType::IIRCN;
}


/* parseNICK
 * Original 12/08/2001 simonb
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parseNICK)
{
   // Rip the nick out, ignoring anything after a space
   String nick = line.nextToken();
   
   // Check we got a nickname from that..
   if (nick.empty()) {
      sendNumeric(RegistrationNumerics::ERR_NONICKNAMEGIVEN);
      return;
   }
   
   // Firstly, make sure the nickname is within acceptable limits (size/chars)
//   if (!User::okName(nick)) {
//      handler->sendNumeric(Numerics::ERR_ERRONEUSNICKNAME, 0,
//			   nick + " :" +
//			   Lang::lang(LangTags::L_ERR_ERRONEUSNICKNAME));
//      return;
//   }
   
   // Check that the nickname is not used by a service (from service list)
   ///////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////
   
   // Check for nicks that are not allowed here (from config)
//   String reason = Daemon::failedNickname(nick);
//   if (reason.length()) {
//      handler->
//	sendNumeric(Numerics::ERR_ERRONEUSNICKNAME, 0,
//		    nick + " :" + 
//		    Lang::lang(LangTags::L_ERR_ERRONEUSNICKNAME) + " (" +
//		    reason + ')');
//      return;
//   }
   
   // Check that the nickname is not already in use
//   if (Daemon::getUser(nick)) {
//      handler->sendNumeric(Numerics::ERR_NICKNAMEINUSE, 0,
//			   nick + " :" +
//			   Lang::lang(LangTags::L_ERR_NICKNAMEINUSE));
//      return;
//   }
   
   // If we got here, the nick was ok - allow it
   registrantData.nickname = nick;
# ifdef KINE_DEBUG_PSYCHO
   debug(" -=>         Nick: " + registrantData.nickname);
# endif
   
   // Do we need to send a ping out?
   if ((pongsLeft > 0) && (pongMatch.empty())) {
      sendPing();
   }
}


/* parsePASS
 * Original 31/08/2001 simonb
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parsePASS)
{
   // Have we already got the password?
   if (!registrantData.password.empty()) {
      // Drop this connection..
      connection.goodbye();
      return;
   }
   
   // Grab the password from the line (we do not care if it is blank)
   registrantData.password = line.nextColonToken();
   
#ifdef KINE_DEBUG_PSYCHO
   debug(" -=>     Password: " + registrantData.password);
#endif
   
   // Is there anything else on the line we should know about?
   if (line.hasMoreTokens()) {
      registrantData.passwordKludge = line.rest();
#ifdef KINE_DEBUG_PSYCHO
      debug(" -=>  PASS Kludge: " + registrantData.passwordKludge);
#endif
   }
}


/* parsePONG
 * Original 16/08/2001 simonb
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parsePONG)
{
   // Were we expecting a pong reply?
   if (pongsLeft == 0) {
      // Drop this connection..
      connection.goodbye();
      return;
   }
   
   // Make sure this pong is valid
   if (line.nextColonToken() != pongMatch) {
      // They tried to trick us - drop them off for being naughty
      connection.goodbye();
      return;
   }

   // Lower the pong count
   pongsLeft--;
   
# ifdef KINE_DEBUG_PSYCHO
   debug(" -=>   Pong match: " + pongMatch);
# endif
   
   // Do we need to send another ping?
   if (pongsLeft > 0) {
      // Send another ping out
      sendPing();
   }
}


/* parseQUIT
 * Original 12/08/2001 simonb
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parseQUIT)
{
   /* Close the connection. No goodbye or anything to prevent abuse. It is
    * very rare for a connection to use the QUIT command during registration,
    * if ever.
    */
   connection.goodbye();
}


/* parseSERVER
 * Original 12/08/2001 simonb
 * Note: Due to the nature of the diversity assosicated with this command, we
 *       must parse this in a careful nature
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parseSERVER)
{
   // Does the port this person connected to accept this type of connection?
   if (!listener.isFlagSet(Listener::Flags::ALLOW_SERVERS)) {
      // Throw away this connection.. We should report an error here?
      connection.goodbye();
      return;
   }
   
   // Make sure this connection has not already been given a registration mode
   if (registrationType != RegistrationType::NONE) {
      sendNumeric(RegistrationNumerics::ERR_ALREADYREGISTERED);
      connection.goodbye();
      return;
   }
   
   /* Check there are enough tokens, based on the lowest possible a la 
    * RFC1459. However, we consider the info/description field optional.
    */
   if (line.countTokens() < 4) {
      sendNumeric(RegistrationNumerics::ERR_NEEDMOREPARAMS, "SERVER");
      return;
   }

   // Grab the first required value, the server name..
   registrantData.hostname = line.nextToken();
#ifdef KINE_DEBUG_PSYCHO
   debug(" -=>       Server: " + registrantData.hostname);
#endif
   
   // Grab the second required value - hop count
   int hops = line.nextToken().toInt();
#ifdef KINE_DEBUG_PSYCHO
   debug(" -=>         Hops: " + String::convert(hops));
#endif
   
   // Check the hop count, it must be 1, any other value is incorrect
   if (hops != 1) {
      connection.goodbye();
      return;
   }

   // Check what the third variable is here...
   
//   startStamp = line.nextToken().toLong();
//   linkStamp = line.nextToken().toLong();
//   protocol = line.nextToken();
//   realname = 
//     line.nextColonToken().substr(0, 
//				  connection.getDaemon().getConfig().
//				  getOptionsLimitsServersMaxDescriptionLength());
   
//#ifdef KINE_DEBUG_PSYCHO
//   // Send what we got to the debugging output
//   debug(" -=>   startStamp: " + String::convert(startStamp));
//   debug(" -=>    linkStamp: " + String::convert(linkStamp));
//   debug(" -=>     Protocol: " + protocol);
//   debug(" -=>         Name: " + realname);
//#endif
   
   // Set the registration mode
   registrationType = RegistrationType::SERVER;
}


/* parseSERVICE
 * Original 28/10/2001 simonb
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parseSERVICE)
{
   // Does the port this person connected to accept this type of connection?
   if (!listener.isFlagSet(Listener::Flags::ALLOW_SERVICES)) {
      // Throw away this connection.. We should report an error here?
      connection.goodbye();
      return;
   }
   
   // Make sure this connection has not already been given a registration mode
   if (registrationType != RegistrationType::NONE) {
      sendNumeric(RegistrationNumerics::ERR_ALREADYREGISTERED);
      connection.goodbye();
      return;
   }
   
   // Check there are enough tokens
   if (line.countTokens() < 7) {
      sendNumeric(RegistrationNumerics::ERR_NEEDMOREPARAMS, "SERVICE");
      return;
   }

   // Grab the service name (nickname/whatever)
   String name = line.nextToken();

   // Firstly, make sure the nickname is within acceptable limits (size/chars)
//   if (!User::okName(name)) {
//      handler->sendNumeric(Numerics::ERR_ERRONEUSNICKNAME, 0,
//			   name + " :" +
//			   Lang::lang(LangTags::L_ERR_ERRONEUSNICKNAME));
//      return;
//   }
 
   // Make sure that the nickname is on the service list
   ///////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////
   
   // Ok, that name should be ok
   registrantData.username = name;
 
   // Rip out the rest of the variables
   (void)line.nextToken(); // ignored.. (RFC-2812: reserved)
   registrantData.distribution = line.nextToken();
   (void)line.nextToken(); // ignored.. (RFC-2812: type)
   (void)line.nextToken(); // ignored (RFC-2812: reserved)
   registrantData.realname = 
     line.nextColonToken().substr(0, 
				  connection.getDaemon().getConfig().
				  getOptionsLimitsUsersMaxRealNameLength());
   
#ifdef KINE_DEBUG_PSYCHO
   // Send what we got to the debugging output
   debug(" -=>      Service: " + registrantData.username);
   debug(" -=> Distribution: " + registrantData.distribution);
   debug(" -=>  Description: " + registrantData.realname);
#endif
   
   // Set the registration mode
   registrationType = RegistrationType::SERVICE;
}


/* parseUSER
 * Original 12/08/2001 simonb
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parseUSER)
{
   // Does the port this person connected to accept this type of connection?
   if (!listener.isFlagSet(Listener::Flags::ALLOW_USERS)) {
      // Throw away this connection.. We should report an error here?
      connection.goodbye();
      return;
   }
   
   // Make sure this connection has not already been given a registration mode
   if (registrationType != RegistrationType::NONE) {
      sendNumeric(RegistrationNumerics::ERR_ALREADYREGISTERED);
      connection.goodbye();
      return;
   }
   
   // Check there are enough tokens
   if (line.countTokens() < 5) {
      sendNumeric(RegistrationNumerics::ERR_NEEDMOREPARAMS, "USER");
      return;
   }
   
   // Rip the command apart
   registrantData.username = line.nextToken();
   registrantData.modes = line.nextToken();
   registrantData.hostname = line.nextToken();
   registrantData.realname = 
     line.nextColonToken().substr(0, 
				  connection.getDaemon().getConfig().
				  getOptionsLimitsUsersMaxRealNameLength());

#ifdef KINE_DEBUG_PSYCHO
   // Output what we got for debugging purposes
   debug(" -=>         User: " + registrantData.username);
   debug(" -=>        Modes: " + registrantData.modes);
   debug(" -=>     Realname: " + registrantData.realname);
#endif
   
   // Set the registration mode
   registrationType = RegistrationType::CLIENT;
}


/* handleInput - Handle incoming data
 * Original 11/08/2001 simonb
 * Note: This could be more efficient :(
 */
void Registrar::handleInput(std::stringstream& data)
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

	 // Check if the buffer has something in it (perhaps a command?)
	 if (!buffer.empty()) {
	    // Hand the data over to the parser (as a single line
	    parseLine(buffer);
	    
	    // Clear the buffer
	    buffer.clear();
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
	 if (buffer.length() > 510) {
#ifdef KINE_DEBUG_PSYCHO
	    debug("Registrar::handleInput() - "
		  "Inordinate amount of incoming data");
#endif
	    connection.goodbye();
	 }
	 
	 // Just add the char to the buffer
	 buffer += (char)data.get();
      }
   }
}


/* withdrawOutput - Remove up to the amount given of octets from the queue
 * Original 28/09/2002 simonb
 * Note: This could be more efficient :(
 */
std::string Registrar::withdrawOutput(AISutil::Socket::blockSize_type amount)
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
