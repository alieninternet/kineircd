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
   if (nickname.empty()) {
      output << " *";
   } else {
      output << ' ' << nickname;
   }

   // Terminate the line according to RFC1459
   output << "\r\n";
   
   // Throw the line onto the output queue
   outputQueue.push(output.str());
}


/* sendNumeric - Send a numeric (including payload)
 * Original 12/08/2001 simonb
 */
void Registrar::sendNumeric(const RegistrationNumerics::numeric_type numeric,
			    const char* data)
{
   std::ostringstream output;
   
   /* Output the server name and the numeric (note, we do not pre-pad the
    * numeric because all registration numerics are over 100 anyway!
    */
   output <<
     ':' << connection.getDaemon().getConfig().getOptionsServerName() <<
     ' ' << (int)numeric;
   
   // Determine the appropriate nickname.
   if (nickname.empty()) {
      output << " * ";
   } else {
      output << ' ' << nickname << ' ';
   }

   // Output the payload and terminate the line
   output << data << "\r\n";
   
   // Throw the line onto the output queue
   outputQueue.push(output.str());
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
#ifdef KINE_DEBUG_PSYCHO
	 debug("Registrar::parseLine() <- Command: " + command);
#endif
	 found = true;
	 (this->*(commandTable[i].handler))(st);
	 break;
      }
   }
}


/* parseCAPAB - Requested capabilities
 * Original 16/08/2001 simonb
 * Note: Capability formatting depends on the protocol usually, so we
 *       don't deal with it here but rather just give it to the protocol
 *       handler upon initialisation for it to grok.
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parseCAPAB)
{
//   // Make sure we were given at least one parameter
//   if (!(tokens->countTokens() >= 2)) {
//      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS, 0,
//			   "CAPAB :" + 
//			   Lang::lang(LangTags::L_ERR_NEEDMOREPARAMS));
//      return;
//   }
//
//   /* Run through the requested capabilities list and look for stuff we can
//    * recognise
//    */
//   for (String ability = tokens->nextToken(); ability.length();
//	ability = tokens->nextToken()) {
//#ifdef KINE_DEBUG_EXTENDED
//      debug(" -=>  Capability: " + ability);
//#endif
//   }
}


/* parseIIRCN
 * Original 05/08/2002 simonb
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parseIIRCN)
{
   // Does the port this person connected to accept this type of connection?
   if (!(listener.getFlags() & Listener::Flags::ALLOW_NETWORKS)) {
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
   nickname = nick;
# ifdef KINE_DEBUG_PSYCHO
   debug(" -=>         Nick: " + nick);
# endif
   
//# ifdef USER_CONNECTION_PINGPONG
//   // Make up some dodgey random stuff with xor!
//   handler->pingpong = 
//     String::printf("%08lX",
//		    ((unsigned long)Daemon::getTime() ^
//		     (unsigned long)(((0xFFFFFFFE + 1.0) * rand()) / 
//				     RAND_MAX)));
   
   // Send a PING out to make sure we are dealing with a 'real' client
//   handler->
//     sendGeneric("NOTICE",
//		 String::printf((char *)Lang::L_PINGPONG_NOTICE,
//				handler->pingpong.c_str(), 
//				handler->pingpong.c_str(),
//				Daemon::myServer()->getHostname().c_str()));
//   handler->getConnection()->sendRaw("PING :" + handler->pingpong + "\r\n");
//# endif
//#else
//   // We are not allowed to accept client connections, so bye bye!
//   handler->getConnection()->goodbye();
//   // ... maybe we should have been a little friendlier?
//#endif
}


/* parsePASS
 * Original 31/08/2001 simonb
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parsePASS)
{
   // Have we already got the password?
   if (!password.empty()) {
      // Drop this connection..
      connection.goodbye();
      return;
   }
   
   // Grab the password from the line (we do not care if it is blank)
   password = line.nextColonToken();
   
#ifdef KINE_DEBUG_PSYCHO
   debug(" -=>     Password: " + password);
#endif
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
   if (line.nextColonToken() == pongMatch) {
      pongsLeft--;
   }
   
   // Do we need to send another pong?
   if (pongsLeft > 0) {
      // Send another pong.
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
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parseSERVER)
{
   // Does the port this person connected to accept this type of connection?
   if (!(listener.getFlags() & Listener::Flags::ALLOW_SERVERS)) {
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
      sendNumeric(RegistrationNumerics::ERR_NEEDMOREPARAMS, "SERVER");
      return;
   }

   // Grab the variables
   username = line.nextToken();
   int hops = line.nextToken().toInt();
   startStamp = line.nextToken().toLong();
   linkStamp = line.nextToken().toLong();
   protocol = line.nextToken();
//   realname = 
//     line.nextColonToken().substr(0, 
//				  connection.getDaemon().getConfig().
//				  getOptionsLimitsServersMaxDescriptionLength());
   
#ifdef KINE_DEBUG_PSYCHO
   // Send what we got to the debugging output
   debug(" -=>       Server: " + username);
   debug(" -=>         Hops: " + String::convert(hops));
   debug(" -=>   startStamp: " + String::convert(startStamp));
   debug(" -=>    linkStamp: " + String::convert(linkStamp));
   debug(" -=>     Protocol: " + protocol);
   debug(" -=>         Name: " + realname);
#endif
   
   // Check the integers variables are ok
   if ((hops != 1) || (startStamp <= 0) || (linkStamp <= 0)) {
#ifdef KINE_DEBUG_EXTENDED
      debug("Variables are invalid!");
#endif
      connection.goodbye();
      return;
   }
   
   // Set the registration mode
   registrationType = RegistrationType::SERVER;
}


/* parseSERVICE
 * Original 28/10/2001 simonb
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parseSERVICE)
{
   // Does the port this person connected to accept this type of connection?
   if (!(listener.getFlags() & Listener::Flags::ALLOW_SERVICES)) {
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
   username = name;
 
   // Rip out the rest of the variables
   (void)line.nextToken(); // ignored.. (RFC-2812: reserved)
   distribution = line.nextToken();
   (void)line.nextToken(); // ignored.. (RFC-2812: type)
   (void)line.nextToken(); // ignored (RFC-2812: reserved)
   realname = 
     line.nextColonToken().substr(0, 
				  connection.getDaemon().getConfig().
				  getOptionsLimitsUsersMaxRealNameLength());
   
#ifdef KINE_DEBUG_PSYCHO
   // Send what we got to the debugging output
   debug(" -=>      Service: " + username);
   debug(" -=> Distribution: " + distribution);
   debug(" -=>  Description: " + realname);
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
   if (!(listener.getFlags() & Listener::Flags::ALLOW_USERS)) {
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
   username = line.nextToken();
   modes = line.nextToken();
   (void)line.nextToken(); // We ignore this one, we will get our own host
   realname = 
     line.nextColonToken().substr(0, 
				  connection.getDaemon().getConfig().
				  getOptionsLimitsUsersMaxRealNameLength());

#ifdef KINE_DEBUG_PSYCHO
   // Output what we got for debugging purposes
   debug(" -=>         User: " + username);
   debug(" -=>        Modes: " + modes);
   debug(" -=>     Realname: " + realname);
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
