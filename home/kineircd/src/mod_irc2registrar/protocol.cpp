/* $Id$
 * Handle registration (before handling a real protocol)
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
#include "kineircd/kineircdconf.h"

#include <sstream>
#include <iomanip>
#include <aisutil/string/string.h>

#include "kineircd/protocolinfo.h"
#include "kineircd/daemon.h"
#include "libkineircd/registrar.h"
#include "libkineircd/regnumerics.h"
#include "libkineircd/debug.h"

using namespace Kine;
using AISutil::String;


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
   // Set the expectant pong result string appropriately
   pongMatch = 
     Utils::baseXStr((unsigned long)(((0xFFFFFFFE + 1.0) * rand()) / RAND_MAX),
		     36);

   // Output the ping
   outputQueue.push("PING :" + pongMatch + "\r\n");
}


/* parseMessage - Appropriately parse a protocol message
 * Original 12/08/2001 simonb
 */
void Registrar::parseMessage(const std::string& origin,
			     const std::string& destination,
			     const std::string& command,
			     const Kine::LibIRC2::Protocol::parameters_type&
			     parameters)
{
   bool found = false;

   // Run through the list and find a function..
   for (int i = 0; commandTable[i].command != 0; i++) {
      if (command == commandTable[i].command) {
	 found = true;
	 (this->*(commandTable[i].handler))(parameters);
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
      
      
    case RegistrationType::USER:
      // Make sure all required fields have been given..
      if (registrantData.name.empty() ||
	  registrantData.username.empty() ||
	  registrantData.hostname.empty() ||
	  (pongsLeft > 0)) {
	 return;
      }
      
      /* If the protocol field is empty, we must make a presumption on what
       * protocol to use - that is not a good thing, but unfortunately nobody
       * considered multiple user protocols until recently..
       */
      if (registrantData.protocol.empty()) {
	 protocolInfo =
	   daemon().findProtocol(ProtocolInfo::Type::USER,
				 config().
				 getOptionsRegistrarUserProtocolDefault());
      } else {
	 protocolInfo = 
	   daemon().findProtocol(ProtocolInfo::Type::USER,
				 registrantData.protocol.toUpper());
      }

      // Check if we found something..
      if (protocolInfo == 0) {
	 // Complain
	 sendError("Requested user protocol unavailable");
	 return;
      }

      // Go onto the next stage (fire the protocol up)
      break;
      
      
    case RegistrationType::IIRCN:
      // Make sure all required fields have been given..
      if (registrantData.password.empty() ||
	  registrantData.name.empty() ||
	  registrantData.hostname.empty() ||
	  registrantData.protocol.empty() ||
	  registrantData.linkStamp <= 0) {
	 return;
      }
      
      /* Since we know IIRCN protocols always have a protocol name, finding
       * the appropriate protocol is easy.
       */
      if ((protocolInfo = 
	   daemon().findProtocol(ProtocolInfo::Type::NETWORK,
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
	  registrantData.name.empty() ||
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
   Kine::Protocol* const newProtocol = 
     protocolInfo->createProtocol(registrantData, connection, inputQueue,
				  output);
      
   // That protocol instance may not have been created, so let's just check..
   if (newProtocol != 0) {
#ifdef KINE_DEBUG_PSYCHO
      std::ostringstream out;
      out << "Registrar::parseLine() - Doing protocol hand-over (" << this <<
	" to " << newProtocol << ')';
      debug(out.str());
#endif
      
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
   if (parameters.empty()) {
      sendNumeric(RegistrationNumerics::ERR_NEEDMOREPARAMS, "CAPAB");
      return;
   }

//   // Append the capability to the capabilities vector
//   registrantData.capabilities.push_back(line.rest());

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
   if (parameters.size() < 6) {
      sendNumeric(RegistrationNumerics::ERR_NEEDMOREPARAMS, "IIRCN");
      return;
   }
   
   // Check that the 'from network' field is * - it must be during registration
   if (parameters[1] != "*") {
#ifdef KINE_DEBUG_PSYCHO
      debug("Registrar::parseIIRCN() - Invalid: from network != \"*\"");
#endif
      connection.goodbye();
      return;
   }
   
   // Rip out the data, then..
   registrantData.name = parameters[0];
   registrantData.hostname = parameters[2];
   registrantData.protocol = parameters[3].toUpper();
   registrantData.linkStamp = parameters[4].toLong();
   
#ifdef KINE_DEBUG_PSYCHO
   // Output debugging info
   debug(" -=>      Network: " + registrantData.name);
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
   
   if (parameters.size() > 5) {
      registrantData.realname = parameters[5];
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
   // Check the parameter list, there must be one parameter at least..
   if (parameters.empty()) {
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
   registrantData.name = parameters[0];
# ifdef KINE_DEBUG_PSYCHO
   debug(" -=>         Nick: " + registrantData.name);
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
   
   // We need at least one parameter here (the password itself, duh!)
   if (parameters.empty()) {
      sendNumeric(RegistrationNumerics::ERR_NEEDMOREPARAMS, "PASS");
      return;
   }
   
   // Grab the password from the line (we do not care if it is blank)
   registrantData.password = parameters[0];
   
#ifdef KINE_DEBUG_PSYCHO
   debug(" -=>     Password: " + registrantData.password);
#endif
   
   // Is there anything else on the line we should know about?
   if (parameters.size() > 1) {
//      registrantData.passwordKludge = line.rest();
//#ifdef KINE_DEBUG_PSYCHO
//      debug(" -=>  PASS Kludge: " + registrantData.passwordKludge);
//#endif
   }
}


/* parsePONG
 * Original 16/08/2001 simonb
 */
KINE_LIB_REGISTRAR_FUNCTION(Registrar::parsePONG)
{
   // Were we expecting a pong reply? Is this pong empty?
   if ((pongsLeft == 0) || (parameters.empty())) {
      // Drop this connection..
      connection.goodbye();
      return;
   }

   // Make sure this pong is valid
   if (parameters[0] != pongMatch) {
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
    * anyway..
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
   if (parameters.size() < 3) {
      sendNumeric(RegistrationNumerics::ERR_NEEDMOREPARAMS, "SERVER");
      return;
   }

   // Grab the first required value, the server name..
   registrantData.hostname = parameters[0];
#ifdef KINE_DEBUG_PSYCHO
   debug(" -=>       Server: " + registrantData.hostname);
#endif
   
   // Grab the second required value - hop count
   int hops = parameters[1].toInt();
#ifdef KINE_DEBUG_PSYCHO
   debug(" -=>         Hops: " + parameters[1]);
#endif
   
   // Check the hop count, it must be 1, any other value is incorrect
   if (hops != 1) {
      // Oh well - nice try.. We don't like servers which want to lie to us :(
      connection.goodbye();
      return;
   }

   // Check what the third variable is here...
   
//   startStamp = line.nextToken().toLong();
//   linkStamp = line.nextToken().toLong();
//   protocol = line.nextToken();
//   realname = 
//     line.nextColonToken().substr(0, 
//				  config().
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
   if (parameters.size() < 6) {
      sendNumeric(RegistrationNumerics::ERR_NEEDMOREPARAMS, "SERVICE");
      return;
   }

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
   
   // Ok, rip out the variables...
   registrantData.username = parameters[0];
   registrantData.distribution = parameters[2];
   
   // If there's a real-name field, we should grab that one too
   if (parameters.size() > 5) {
      registrantData.realname = 
	parameters[5].substr(0,
			     config().getOptionsLimitsUsersMaxRealNameLength());
   }
   
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
   if (parameters.size() < 4) {
      sendNumeric(RegistrationNumerics::ERR_NEEDMOREPARAMS, "USER");
      return;
   }
   
   // Rip the command apart
   registrantData.username = parameters[0];
   registrantData.modes = parameters[1];
   registrantData.hostname = parameters[2];
   
   // If there's a real-name field, grab it too
   if (parameters.size() > 3) {
      registrantData.realname =
	parameters[3].substr(0,
			     config().getOptionsLimitsUsersMaxRealNameLength());
   }

#ifdef KINE_DEBUG_PSYCHO
   // Output what we got for debugging purposes
   debug(" -=>         User: " + registrantData.username);
   debug(" -=>        Modes: " + registrantData.modes);
   debug(" -=>     Realname: " + registrantData.realname);
#endif
   
   // Set the registration mode
   registrationType = RegistrationType::USER;
}
