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

#include <sstream>
#include <iomanip>
#include <aisutil/string/string.h>
#include <aisutil/utils.h>
#include <kineircd/protocolinfo.h>
#include <kineircd/daemon.h>
#include <kineircd/registry.h>

#include "mod_irc2registrar/protocol.h"
#include "mod_irc2registrar/language.h"
#ifdef KINE_DEBUG
# include "mod_irc2registrar/debug.h"
#endif

using namespace Kine::mod_irc2registrar;
using AISutil::String;


// Our "official" protocol name
const char* const Protocol::protocolName =
  "irc2registrar";


// Master command table (zero terminated, ordered alphabetically)
const Protocol::commandTable_type Protocol::commandTable[] = {
     { "CAPAB",		&Protocol::parseCAPAB },
     { "IIRCN",		&Protocol::parseIIRCN },
     { "NICK",		&Protocol::parseNICK },
     { "PASS",		&Protocol::parsePASS },
     { "PONG",		&Protocol::parsePONG },
     { "QUIT",		&Protocol::parseQUIT },
     { "SERVER",	&Protocol::parseSERVER },
     { "SERVICE",	&Protocol::parseSERVICE },
     { "USER",		&Protocol::parseUSER },
     { 0, 0 }
};


/* sendPing - Send a ping with some unpredictable data
 * Original 12/08/2001 simonb
 */
void Protocol::sendPing(void)
{
   // Set the expectant pong result string appropriately
   pongMatch = 
     AISutil::Utils::baseXStr((unsigned long)
			      (((0xFFFFFFFE + 1.0) * rand()) / RAND_MAX),
			      36);

   // Output the ping
   sendMessage("PING", ":" + pongMatch + "\r\n");
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
   bool found = false;

   // Run through the list and find a function..
   for (int i = 0; commandTable[i].command != 0; ++i) {
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
	 sendError(GETLANG(irc2registrar_ERROR_PROTOCOL_UNAVAILABLE_USER));
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
	 sendError(GETLANG(irc2registrar_ERROR_PROTOCOL_UNAVAILABLE_IIRC));
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
      sendError(GETLANG(irc2registrar_ERROR_PROTOCOL_UNAVAILABLE_SERVER));
      return;
      
      
    case RegistrationType::SERVICE:
      // Make sure all required fields have been given..
      if (registrantData.password.empty() ||
	  registrantData.name.empty() ||
	  registrantData.distribution.empty()) {
	 return;
      }
       
      // No support yet...
      sendError(GETLANG(irc2registrar_ERROR_PROTOCOL_UNAVAILABLE_SERVICE));
      return;
   }
   
   // If we got here, we are likely to switch to the newly created protocol.
#ifdef KINE_DEBUG
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
      
#ifdef KINE_DEBUG
   std::ostringstream out;
   out << "mod_irc2registrar::Protocol::parseLine() - Attempting protocol "
     "hand-over (" << this << " to " << newProtocol << ')';
   debug(out.str());
#endif
      
   // That protocol instance may not have been created, so let's just check..
   if (newProtocol != 0) {
      // Do the protocol hand-over
      connection.setProtocol(*newProtocol);
      
      // Delete ourself.. (suicide? ;)
      delete this;
      
      // Bye bye!
      return;
   }

   // Uh oh - the protocol was not created..
   sendError(GETLANG(irc2registrar_ERROR_PROTOCOL_INIT_FAIL));
}


/* parseCAPAB - Requested capabilities
 * Original 16/08/2001 simonb
 * Note: Capability formatting depends on the protocol usually, so we
 *       don't deal with it here but rather just give it to the protocol
 *       handler upon initialisation for it to grok.
 */
KINE_MOD_REGISTRAR_FUNCTION(Protocol::parseCAPAB)
{
   // Make sure we were given at least one parameter
   if (parameters.empty()) {
      sendNumeric(LibIRC2::Numerics::ERR_NEEDMOREPARAMS,
		  "CAPAB",
		  GETLANG(irc2_ERR_NEEDMOREPARAMS));
      return;
   }

//   // Append the capability to the capabilities vector
//   registrantData.capabilities.push_back(line.rest());

#ifdef KINE_DEBUG
   std::ostringstream out;
   out << "mod_irc2registrar: -=>   Capability: " <<
     registrantData.capabilities.back();
   debug(out.str());
#endif
}


/* parseIIRCN
 * Original 05/08/2002 simonb
 */
KINE_MOD_REGISTRAR_FUNCTION(Protocol::parseIIRCN)
{
   // Does the port this person connected to accept this type of connection?
   if (!listener.isFlagSet(Listener::Flags::ALLOW_NETWORKS)) {
      // Throw away this connection..
      sendError(GETLANG(irc2registrar_ERROR_CONN_DISALLOWED_IIRC));
      connection.goodbye();
      return;
   }
   
   // Make sure this connection has not already been given a registration mode
   if (registrationType != RegistrationType::NONE) {
      sendNumeric(LibIRC2::Numerics::ERR_ALREADYREGISTERED,
		  GETLANG(irc2_ERR_ALREADYREGISTERED));
      connection.goodbye();
      return;
   }

   // Check there are enough tokens. We consider the description field optional
   if (parameters.size() < 6) {
      sendNumeric(LibIRC2::Numerics::ERR_NEEDMOREPARAMS,
		  "IIRCN",
		  GETLANG(irc2_ERR_NEEDMOREPARAMS));
      return;
   }
   
   // Check that the 'from network' field is * - it must be during registration
   if (parameters[1] != "*") {
#ifdef KINE_DEBUG
      debug("mod_irc2registrar::Protocol::parseIIRCN() - Invalid: from "
	    "network != \"*\"");
#endif
      sendError(GETLANG(irc2registrar_ERROR_BROKEN_REGISTRATION_DATA));
      connection.goodbye();
      return;
   }
   
   // Rip out the data, then..
   registrantData.name = parameters[0];
   registrantData.hostname = parameters[2];
   registrantData.protocol = parameters[3].toUpper();
   registrantData.linkStamp = parameters[4].toLong();
   
#ifdef KINE_DEBUG
   // Output debugging info
   debug("mod_irc2registrar: -=>      Network: " +
	 registrantData.name);
   debug("mod_irc2registrar: -=> Gateway Host: " +
	 registrantData.hostname);
   debug("mod_irc2registrar: -=>   IIRC Proto: " +
	 registrantData.protocol);
   debug("mod_irc2registrar: -->   Time stamp: " +
	 String::convert(registrantData.linkStamp));
#endif
   
   // Check the linkstamp, it must be greater than 0..
   if (registrantData.linkStamp <= 0) {
#ifdef KINE_DEBUG
      debug("mod_irc2registrar::Protocol::parseIIRCN() - Invalid: "
	    "timestamp <= 0");
#endif
      sendError(GETLANG(irc2registrar_ERROR_BROKEN_REGISTRATION_DATA));
      connection.goodbye();
      return;
   }
   
   if (parameters.size() > 5) {
      registrantData.description = parameters[5];
#ifdef KINE_DEBUG
      debug("mod_irc2registrar: -=>  Description: " +
	    registrantData.description);
#endif
   }
   
   // Set the registration mode
   registrationType = RegistrationType::IIRCN;
}


/* parseNICK
 * Original 12/08/2001 simonb
 */
KINE_MOD_REGISTRAR_FUNCTION(Protocol::parseNICK)
{
   // Check the parameter list, there must be one parameter at least..
   if (parameters.empty()) {
      sendNumeric(LibIRC2::Numerics::ERR_NONICKNAMEGIVEN,
		  GETLANG(irc2_ERR_NONICKNAMEGIVEN));
      return;
   }
   
   // Grab the nickname..
   Name nick = parameters[0];
   
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
   if (registry().findClient(nick)) {
      sendNumeric(LibIRC2::Numerics::ERR_NICKNAMEINUSE,
		  GETLANG(irc2_ERR_NICKNAMEINUSE));
      return;
   }
   
   // If we got here, the nick was ok - allow it
   registrantData.name = nick;
#ifdef KINE_DEBUG
   debug("mod_irc2registrar: -=>         Nick: " +
	 registrantData.name);
#endif
   
   // Do we need to send a ping out?
   if ((pongsLeft > 0) && (pongMatch.empty())) {
      sendPing();
   }
}


/* parsePASS
 * Original 31/08/2001 simonb
 */
KINE_MOD_REGISTRAR_FUNCTION(Protocol::parsePASS)
{
   // Have we already got the password?
   if (!registrantData.password.empty()) {
      // Drop this connection..
      sendError(GETLANG(irc2registrar_ERROR_BROKEN_REGISTRATION_DATA));
      connection.goodbye();
      return;
   }
   
   // We need at least one parameter here (the password itself, duh!)
   if (parameters.empty()) {
      sendNumeric(LibIRC2::Numerics::ERR_NEEDMOREPARAMS,
		  "PASS",
		  GETLANG(irc2_ERR_NEEDMOREPARAMS));
      return;
   }
   
   // Grab the password from the line (we do not care if it is blank)
   registrantData.password = parameters[0];
   
#ifdef KINE_DEBUG
   debug("mod_irc2registrar: -=>     Password: " +
	 registrantData.password);
#endif
   
   // Is there anything else on the line we should know about?
   if (parameters.size() > 1) {
//      registrantData.passwordKludge = line.rest();
//#ifdef KINE_DEBUG
//      debug("mod_irc2registrar: -=>  PASS Kludge: " +
//	    registrantData.passwordKludge);
//#endif
   }
}


/* parsePONG
 * Original 16/08/2001 simonb
 */
KINE_MOD_REGISTRAR_FUNCTION(Protocol::parsePONG)
{
   // Were we expecting a pong reply? Is this pong empty?
   if ((pongsLeft == 0) || (parameters.empty())) {
      // Drop this connection..
      sendError(GETLANG(irc2registrar_ERROR_BAD_PONG));
      connection.goodbye();
      return;
   }

   // Make sure this pong is valid
   if (parameters[0] != pongMatch) {
      // They tried to trick us!
      sendError(GETLANG(irc2registrar_ERROR_BAD_PONG));
      
      // Drop them off for being naughty
      connection.goodbye();
      return;
   }

   // Lower the pong count
   --pongsLeft;
   
# ifdef KINE_DEBUG
   debug("mod_irc2registrar: -=>   Pong match: " +
	 pongMatch);
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
KINE_MOD_REGISTRAR_FUNCTION(Protocol::parseQUIT)
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
KINE_MOD_REGISTRAR_FUNCTION(Protocol::parseSERVER)
{
   // Does the port this person connected to accept this type of connection?
   if (!listener.isFlagSet(Listener::Flags::ALLOW_SERVERS)) {
      // Throw away this connection..
      sendError(GETLANG(irc2registrar_ERROR_CONN_DISALLOWED_SERVER));
      connection.goodbye();
      return;
   }
   
   // Make sure this connection has not already been given a registration mode
   if (registrationType != RegistrationType::NONE) {
      sendNumeric(LibIRC2::Numerics::ERR_ALREADYREGISTERED,
		  GETLANG(irc2_ERR_ALREADYREGISTERED));
      connection.goodbye();
      return;
   }
   
   /* Check there are enough tokens, based on the lowest possible a la 
    * RFC1459. However, we consider the info/description field optional.
    */
   if (parameters.size() < 3) {
      sendNumeric(LibIRC2::Numerics::ERR_NEEDMOREPARAMS,
		  "SERVER",
		  GETLANG(irc2_ERR_NEEDMOREPARAMS));
      return;
   }

   // Grab the first required value, the server name..
   registrantData.hostname = parameters[0];
#ifdef KINE_DEBUG
   debug("mod_irc2registrar: -=>       Server: " +
	 registrantData.hostname);
#endif
   
   // Grab the second required value - hop count
   int hops = parameters[1].toInt();
#ifdef KINE_DEBUG
   debug("mod_irc2registrar: -=>         Hops: " +
	 parameters[1]);
#endif
   
   // Check the hop count, it must be 1, any other value is incorrect
   if (hops != 1) {
      // Oh well - nice try.. We don't like servers which want to lie to us :(
      sendError(GETLANG(irc2registrar_ERROR_BROKEN_REGISTRATION_DATA));
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
   
//#ifdef KINE_DEBUG
//   // Send what we got to the debugging output
//   debug("mod_irc2registrar: -=>   startStamp: " +
//	 String::convert(startStamp));
//   debug("mod_irc2registrar: -=>    linkStamp: " +
//	 String::convert(linkStamp));
//   debug("mod_irc2registrar: -=>     Protocol: " +
//	 protocol);
//   debug("mod_irc2registrar: -=>         Name: " +
//	 realname);
//#endif
   
   // Set the registration mode
   registrationType = RegistrationType::SERVER;
}


/* parseSERVICE
 * Original 28/10/2001 simonb
 */
KINE_MOD_REGISTRAR_FUNCTION(Protocol::parseSERVICE)
{
   // Does the port this person connected to accept this type of connection?
   if (!listener.isFlagSet(Listener::Flags::ALLOW_SERVICES)) {
      // Throw away this connection..
      sendError(GETLANG(irc2registrar_ERROR_CONN_DISALLOWED_SERVICE));
      connection.goodbye();
      return;
   }
   
   // Make sure this connection has not already been given a registration mode
   if (registrationType != RegistrationType::NONE) {
      sendNumeric(LibIRC2::Numerics::ERR_ALREADYREGISTERED,
		  GETLANG(irc2_ERR_ALREADYREGISTERED));
      connection.goodbye();
      return;
   }
   
   // Check there are enough tokens
   if (parameters.size() < 6) {
      sendNumeric(LibIRC2::Numerics::ERR_NEEDMOREPARAMS,
		  "SERVICE",
		  GETLANG(irc2_ERR_NEEDMOREPARAMS));
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
      registrantData.description = 
	parameters[5].substr(0, config().getLimitsUsersMaxRealNameLength());
   }
   
#ifdef KINE_DEBUG
   // Send what we got to the debugging output
   debug("mod_irc2registrar: -=>      Service: " +
	 registrantData.username);
   debug("mod_irc2registrar: -=> Distribution: " +
	 registrantData.distribution);
   debug("mod_irc2registrar: -=>  Description: " +
	 registrantData.description);
#endif
   
   // Set the registration mode
   registrationType = RegistrationType::SERVICE;
}


/* parseUSER
 * Original 12/08/2001 simonb
 */
KINE_MOD_REGISTRAR_FUNCTION(Protocol::parseUSER)
{
   // Does the port this person connected to accept this type of connection?
   if (!listener.isFlagSet(Listener::Flags::ALLOW_USERS)) {
      // Throw away this connection..
      sendError(GETLANG(irc2registrar_ERROR_CONN_DISALLOWED_USER));
      connection.goodbye();
      return;
   }
   
   // Make sure this connection has not already been given a registration mode
   if (registrationType != RegistrationType::NONE) {
      sendNumeric(LibIRC2::Numerics::ERR_ALREADYREGISTERED,
		  GETLANG(irc2_ERR_ALREADYREGISTERED));
      connection.goodbye();
      return;
   }
   
   // Check there are enough tokens
   if (parameters.size() < 4) {
      sendNumeric(LibIRC2::Numerics::ERR_NEEDMOREPARAMS,
		  "USER",
		  GETLANG(irc2_ERR_NEEDMOREPARAMS));
      return;
   }
   
   // Rip the command apart
   registrantData.username = parameters[0];
   registrantData.modes = parameters[1];
   registrantData.hostname = parameters[2];
   
   // If there's a real-name field, grab it too
   if (parameters.size() > 3) {
      registrantData.description =
	parameters[3].substr(0, config().getLimitsUsersMaxRealNameLength());
   }

#ifdef KINE_DEBUG
   // Output what we got for debugging purposes
   debug("mod_irc2registrar: -=>         User: " +
	 registrantData.username);
   debug("mod_irc2registrar: -=>        Modes: " +
	 registrantData.modes);
   debug("mod_irc2registrar: -=>     Hostname: " +
	 registrantData.hostname);
   debug("mod_irc2registrar: -=>  Description: " +
	 registrantData.description);
#endif
   
   // Set the registration mode
   registrationType = RegistrationType::USER;
}
