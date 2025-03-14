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

#include <ctime>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <typeinfo>
#include <aisutil/string.h>
#include <kineircd/mynetwork.h>
#include <kineircd/libirc2/utility.h>
#ifdef KINE_DEBUG_ASSERT
# include <cassert>
#endif

#include "mod_irc2user/protocol.h"
#include "mod_irc2user/lang.h"
#include "mod_irc2user/commands.h"

using namespace Kine::mod_irc2user;
using AIS::Util::String;
using AIS::Util::StringMask;
using AIS::Util::StringTokens;


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_ADMIN
/* handleADMIN
 * Original 27/08/2001 simonb
 * Note: Needs to be turned into a template for all the requests
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleADMIN)
{
   Server* server = 0;
   
   // If there are no parameters, the user was wants us to reply..
   if (parameters.empty()) {
      server = &myServer();
   } else {
      // Look up the server
      server = LibIRC2::Utility::findServerTarget(localiseStr(parameters[0]));
   }
   
   // If we found a server, send the request to that server
   if (server != 0) {
      const Error::error_type requestError = server->requestAdminInfo(user);
      if (requestError != Error::NO_ERROR) {
	 sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNERROR,
		     delocaliseStr(server->getName()),
		     GETLANG(irc2_ERR_UNKNOWNERROR));
      }
      return;
   }
   
   // If we got here, we didn't find the server..
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVER,
	       parameters[0],
	       GETLANG(irc2_ERR_NOSUCHSERVER));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_AWAY
/* handleAWAY
 * Original 13/08/2001 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleAWAY)
{
   static const char* const commandName = "AWAY";
   
   // If there is a parameter, the user is going away
   if (!parameters.empty()) {
      // Set the user 'away'
      if (user.setAway(user, localiseStr(parameters[0])) == Error::NO_ERROR) {
	 return;
      }
   } else {
      // No parameters were given, so the user is now here..
      if (user.setHere(user) == Error::NO_ERROR) {
	 return;
      }
   }

   // What happened?? No idea..
   sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNERROR,
	       commandName,
	       GETLANG(irc2_ERR_UNKNOWNERROR));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_DIE
/* handleDIE
 * Original 14/08/2001 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleDIE)
{
   static const char* const commandName = "DIE";

   // Try to shutdown the server - eek!
   const Error::error_type error =
     myServer().shutdown(user, localiseStr(parameters[0]));
   
   // Check the status of the operation
   if (error != Error::NO_ERROR) {
      // What's the error?
      if (error == Error::PERMISSION_DENIED) {
	 // Complain about not having access to this command
	 sendNumeric(LibIRC2::Numerics::ERR_NOPRIVILEGES,
		     commandName,
		     GETLANG(irc2_ERR_NOPRIVILEGES_SPECIFIC));
	 return;
      } else if (error == Error::TEXT_TOO_SHORT) {
	 // Complain about the reason being too short..
	 sendNumeric(LibIRC2::Numerics::ERR_TEXTTOOSHORT,
		     commandName,
		     GETLANG(irc2_ERR_TEXTTOOSHORT));
	 return;
      } 
      
# ifdef KINE_DEBUG_ASSERT
      // Make sure we're sane..
      assert(error != Error::UNREGISTERED_ENTITY);
# endif

      // Unknown error
      sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNERROR,
		  commandName,
		  GETLANG(irc2_ERR_UNKNOWNERROR));
   }
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_HELP
/* handleHELP
 * Original 13/08/2001 simonb
 * Note: Not the best use on CPU since it's checking a match no matter how
 *       this is called, but since this is very rarely called I decided the
 *       code should be small rather than faster :)
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleHELP)
{
   bool extended = false;
   StringMask mask;
   
   // Check if we weren't given any parameters
   if (parameters.empty()) {
      mask = "*";
   } else {
      // Check if the first char is a '-' (for extended help)
      if (parameters[0][0] == '-') {
	 extended = true;
	 mask = parameters[0].substr(1);
      } else {
	 mask = parameters[0];
      }
   }
   
   // Run through the list of functions
   for (Commands::commandList_type::const_iterator it =
	commands().getCommandList().begin();
	it != commands().getCommandList().end();
	++it) {
      /* Check if this is the command our user was looking for, whether the
       * command has help, and whether the user even has access to this
       * command in the first place!
       */
      if (mask.matches(it->first) && it->second.hasAccess(user)) {
	 // Create a prefix for this command's output
	 const std::wstring prefix(L'\002' + 
				   Languages::toWideStr(it->first) + 
				   L"\002: ");
	 
	 // Send the user the usage help for this function, if we can
	 if (it->second.helpUsage == 0) {
	    user.sendServerNotice(prefix +
				  L_GETLANG(irc2user_HELP_NO_PARAMETERS));
	 } else {
	    user.sendServerNotice(prefix +
				  L_GETLANG_BY_ID(*(it->second.helpUsage)));
	 }
	 
	 // If we are doing extended help, send the extended lines
	 if (extended && (it->second.helpInfo != 0)) {
	    // Send the extended help stuff
	    user.sendServerNotice(prefix +
				  L_GETLANG_BY_ID(*(it->second.helpInfo)));
	 }
      }
   }

   // Tell the user the HELP command has completed its run
   user.sendServerNotice(L_GETLANG(irc2user_HELP_END,
				   localiseStr(mask)));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_INFO
/* handleINFO
 * Original 14/08/2001 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleINFO)
{
   // If there are no parameters, the user was wants us to reply..
   if (parameters.empty()) {
      doINFO(user);
      return;
   }
   
   // Look up the server?
   const std::string& serverName = parameters[0];
   
   // We didn't find the server..
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVER,
	       serverName,
	       GETLANG(irc2_ERR_NOSUCHSERVER));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_ISON
/* handleISON
 * Original 14/08/2001 simonb
 * Note: Should this check to see the maximum output length and not be so
 *       trusting?
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleISON)
{
   // Our reply. We prefix it with a colon so we can output many nicknames
   std::string reply;
   
   const Client* client = 0;
   
   // Run through the parameters (the nicknames we are to check
   for (parameters_type::const_iterator it = parameters.begin(); 
	it != parameters.end(); ++it) {
      // Check if we have that nick in our user or service lists
      if ((client = myNetwork().findClient(localiseStr(*it))) != 0) {
	 // If the reply is not empty, prefix this nickname with a space
	 if (!reply.empty()) {
	    reply += ' ';
	 }
	 
	 // Append the nickname
	 reply += delocaliseStr(client->getNickname());
      }
   }
   
   // Send the reply
   sendNumeric(LibIRC2::Numerics::RPL_ISON,
	       reply);
}
#endif
  
  
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_JOIN
/* handleJOIN
 * Original ? simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleJOIN)
{
   // fix me soon
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHCHANNEL,
	       parameters[0],
	       GETLANG(irc2_ERR_NOSUCHCHANNEL_UNSUPPORTED_TYPE));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_KILL
/* handleKILL
 * Original 22/09/01 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleKILL)
{
   static const char* const commandName = "KILL";
   
   // Find the client to kill
   Client* const victim = myNetwork().findClient(localiseStr(parameters[0]));
   
   // Check
   if (victim != 0) {
      // Attempt to commit the homicide
      const Error::error_type error =
	victim->kill(user, localiseStr(parameters[1]));
      
      // Check the error
      if (error == Error::NO_ERROR) {
	 /* Tell the user the kill was done. Supposedly this reply is
	  * considered to be obsolete, however it's nice to have some sort
	  * of feedback on the command if the user is not listening to
	  * kill notices or can see the client directly.
	  */
//	 if (true /* config */) {
//	    sendNumeric(LibIRC2::Numerics::RPL_KILLDONE,
//			victim->getName(),
//			GETLANG(irc2_RPL_KILLDONE));
//	 }
	 return;
      } else if (error == Error::PERMISSION_DENIED) {
	 // Complain about not having access to this command
	 sendNumeric(LibIRC2::Numerics::ERR_NOPRIVILEGES,
		     commandName,
		     GETLANG(irc2_ERR_NOPRIVILEGES_SPECIFIC));
	 return;
      } else if (error == Error::TEXT_TOO_SHORT) {
	 // Complain about the reason being too short..
	 sendNumeric(LibIRC2::Numerics::ERR_TEXTTOOSHORT,
		     commandName,
		     GETLANG(irc2_ERR_TEXTTOOSHORT));
	 return;
      }
      
# ifdef KINE_DEBUG_ASSERT
      // Make sure we're sane..
      assert(error != Error::UNREGISTERED_ENTITY);
# endif

      // Unknown error..
      sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNERROR,
		  commandName,
		  GETLANG(irc2_ERR_UNKNOWNERROR));
      return;
   }
   
   // There was no such nickname found
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHNICK,
	       parameters[0],
	       GETLANG(irc2_ERR_NOSUCHNICK));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_LANGUAGE
/* handleLANGUAGE
 * Original 26/10/2001 simonb
 * Note: Needs to allow for the configuration option allowing unknown langs.
 *       It also needs duplicates checks (i.e. /language en,en,en = crappy)
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleLANGUAGE)
{
   static const char* const commandName = "LANGUAGE";
   
   // If we do not have any parameters, we will list the languages available
   if (parameters.empty()) {
      // Run through known languages and list them to the client
      for (Languages::languageDataMap_type::const_iterator it =
	   languages().getLanguageDataMap().begin();
	   it != languages().getLanguageDataMap().end();
	   ++it) {
	 // Make the modes string
	 std::string modes;
	 
	 // If this is the default language, add a 'd'
	 if (it->second == languages().getDefaultLanguage()) {
	    modes += 'd';
	 }
	 
	 // If this language appears incomplete, add an 'i'
	 if (it->second->getTagCount() < languages().getHighestTagID()) {
	    modes += 'i';
	 }
	 
	 // If this is the current primary language, add an 'S'
	 if (it->second == user.getLanguageList()[0]) {
	    modes += 'S';
	 }
	 
	 // No modes? Fix it up with a blank thingy
	 if (modes.empty()) {
	    modes = replacementCharacter;
	 }
	 
	 // Send the info on this language
	 sendNumeric(LibIRC2::Numerics::RPL_LANGUAGE,
		     it->second->getLanguageCode(),
		     it->second->getFileRevision(),
		     (it->second->getMaintainer().empty() ?
		      replacementParameter :
		      delocaliseStr(it->second->getMaintainer())),
		     modes,
		     delocaliseStr(it->second->getLanguageNote().empty() ?
				   it->second->getLanguageName() :
				   it->second->getLanguageName() + L" (" +
				   it->second->getLanguageNote() + L")"));
      }
      
      // Send the end of list tag
      sendNumeric(LibIRC2::Numerics::RPL_ENDOF_GENERIC,
		  commandName,
		  GETLANG(irc2_RPL_ENDOF_GENERIC_LANGUAGE));
      return;      
   }

   // Start building a new language data list for the user
   unsigned char languageCount = 0;
   Languages::languageDataList_type languageList;
   StringTokens langCodes(parameters[0]);
   String code;
   Languages::LanguageData* languageData = 0;
   
   // Run over the language codes list we've been given
   while (langCodes.hasMoreTokens()) {
      // Grab this code
      code = langCodes.nextToken(',');

      // Try to find this language, using the given code
      languageData = languages().findByCode(code);

      // Did we find the language?
      if (languageData == 0) {
	 // If the user is not looking for the null language, get cranky
	 if (code.toLower() != Languages::nullLanguageCode) {
	    // Complain
	    sendNumeric(LibIRC2::Numerics::ERR_NOLANGUAGE,
			code,
			GETLANG(irc2_ERR_NOLANGUAGE));
	    
	    // Continue to the next token/code
	    continue;
	 }
      }
      
      // Check for duplicates?
      
      // Are we still within the maximum number of languages limit?
      if (++languageCount > config().getLimitsUsersMaxLanguages()) {
	 // Whinge
	 sendNumeric(LibIRC2::Numerics::ERR_TOOMANYLANGUAGES,
		     (unsigned int)config().getLimitsUsersMaxLanguages(),
		     GETLANG(irc2_ERR_TOOMANYLANGUAGES));
	 return;
      }

      // Add the language to the list
      languageList.push_back(languageData);
   }

   // If the language list is empty, forget about the whole thing..
   if (languageList.empty()) {
      return;
   }
   
   // Set the new language list
   if (user.setLanguageList(user, languageList) == Error::NO_ERROR) {
      return;
   }
   
   // Errm, this is bad..
   sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNERROR,
	       commandName,
	       GETLANG(irc2_ERR_UNKNOWNERROR));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_LUSERS
/* handleLUSERS
 * Original 27/08/2001 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleLUSERS)
{
   // If there are no parameters, the user was wants us to reply..
   if (parameters.empty()) {
      doLUSERS(user);
      return;
   }
   
   // Look up the server?
   const std::string& serverName = parameters[0];
   
   // We didn't find the server..
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVER,
	       serverName,
	       GETLANG(irc2_ERR_NOSUCHSERVER));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_MOTD
/* handleMOTD
 * Original 13/08/2001 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleMOTD)
{
   // If there are no parameters, the user was wants us to reply..
   if (parameters.empty()) {
      doMOTD(user);
      return;
   }
   
   // Look up the server?
   const std::string& serverName = parameters[0];
   
   // We didn't find the server..
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVER,
	       serverName,
	       GETLANG(irc2_ERR_NOSUCHSERVER));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_NAMES
/* handleNAMES
 * Original 15/08/2001 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleNAMES)
{
   // Tokenise the requests
   StringTokens request(parameters.empty() ?
			replacementParameter :
			parameters[0]);

   // Stuff we need during the request loop
   StringMask target;
   const Channel* channel;
   
   // Iterate over the requests
   while (request.hasMoreTokens()) {
      target = request.nextToken(',');
      
      // Find this channel without doing the whole mask checking thingy..
      if ((channel = myNetwork().findChannel(localiseStr(target))) != 0) {
	 // Output the names list for this channel..
	 sendNames(*channel);
	 continue;
      }
   }
   
   // Send end of names list
   sendNumeric(LibIRC2::Numerics::RPL_ENDOFNAMES,
	       request,
	       GETLANG(irc2_RPL_ENDOFNAMES));
}
#endif


# ifdef KINE_MOD_IRC2USER_HAVE_CMD_NETWORKS
/* handleNETWORKS
 * Original 01/05/2003 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleNETWORKS)
{
   static const char* const commandName = "NETWORKS";
   
   // If there are no parameters, the user simply wants to list the networks
   if (parameters.empty()) {
      // Send the end of networks list numerics
      sendNumeric(LibIRC2::Numerics::RPL_ENDOF_GENERIC,
		  commandName,
		  GETLANG(irc2_RPL_ENDOF_GENERIC_NETWORKS));
      return;
   }
   
   // Check permissions!   
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_NICK
/* handleNICK
 * Original 24/08/2001 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleNICK)
{
   // Make sure we got a nickname..
   if (parameters.empty()) {
      sendNumeric(LibIRC2::Numerics::ERR_NONICKNAMEGIVEN,
		  GETLANG(irc2_ERR_NONICKNAMEGIVEN));
      return;
   }
   
   
   // If the user is an IRC operator, they can get around the next checks..
   if (!user.isOperator()) {
      /* Make sure this user is not on a channel marked with the event flag.
       * This only effects users that are not opped or voiced in that channel
       */
      if (false) {
	 sendNumeric(LibIRC2::Numerics::ERR_EVENTNICKCHANGE,
		     "#channel",
		     GETLANG(irc2_ERR_EVENTNICKCHANGE));
	 return;
      }
      
      // Check if the nickname is on a fail list somewhere
      if (false) {
	 sendNumeric(LibIRC2::Numerics::ERR_ERRONEUSNICKNAME,
		     parameters[0],
		     GETLANG(irc2_ERR_ERRONEUSNICKNAME_PROHIBITED//,
			     /* reason */));
	 return;
      }
   }

   // Try to do the nick change, working out the errors returned
   switch (user.changeNickname(user, localiseStr(parameters[0]))) {
    // All went well, nothing left to do
    case Error::NO_ERROR:
      return;
      
    // The nickname was erroneous (bad chars)
    case Error::NAME_HAS_BAD_CHARS:
      sendNumeric(LibIRC2::Numerics::ERR_ERRONEUSNICKNAME,
		  parameters[0],
		  GETLANG(irc2_ERR_ERRONEUSNICKNAME_BAD_CHARS));
      return;
      
    // The nickname is already in use by someone else
    case Error::NAME_IS_IN_USE:
      sendNumeric(LibIRC2::Numerics::ERR_NICKNAMEINUSE,
		  parameters[0],
		  GETLANG(irc2_ERR_NICKNAMEINUSE));
      return;
      
    // The nickname was erroneous (too long)
    case Error::NAME_TOO_LONG:
      sendNumeric(LibIRC2::Numerics::ERR_ERRONEUSNICKNAME,
		  parameters[0],
		  GETLANG(irc2_ERR_ERRONEUSNICKNAME_TOO_LONG,
			  Languages::toWideStr(String::convert((unsigned int)config().
							       getLimitsUsersMaxNickNameLength()))));
      return;

    // The nickname was erroneous (started with a digit)
    case Error::NICKNAME_BEGINS_WITH_DIGIT:
      sendNumeric(LibIRC2::Numerics::ERR_ERRONEUSNICKNAME,
		  parameters[0],
		  GETLANG(irc2_ERR_ERRONEUSNICKNAME_BEGINS_WITH_DIGIT));
      return;
      
    // NFI what happened, but the nickname was invalid anyway
    default:
      sendNumeric(LibIRC2::Numerics::ERR_ERRONEUSNICKNAME,
		  parameters[0],
		  GETLANG(irc2_ERR_ERRONEUSNICKNAME));
   }
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_NOTICE
/* handleNOTICE
 * Original 01/09/2001 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleNOTICE)
{
   doMessage(parameters, true);
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_OPER
/* handleOPER
 * Original ? simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleOPER)
{
   // fix me when there's actually IRC operator code :)
   sendNumeric(LibIRC2::Numerics::ERR_NOOPERHOST,
	       GETLANG(irc2_ERR_NOOPERHOST));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_PART
/* handlePART
 * Original ? simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handlePART)
{
   // fix me soon
   sendNumeric(LibIRC2::Numerics::ERR_NOTONCHANNEL,
	       GETLANG(irc2_ERR_NOTONCHANNEL));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_PING
/* handlePING
 * Original 25/08/2001 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handlePING)
{
   // If we were given a parameter, send it back (simple as that!)
   if (!parameters.empty()) {
      // Send the reply (pong) in full form
      sendMessageFrom(delocaliseStr(myServer().getName()), "PONG",
		      delocaliseStr(myServer().getName()),
		      parameters[0]);
      return;
   }
   
   // Umm, return a complaint as per standards, even if it is kinda wrong
   sendNumeric(LibIRC2::Numerics::ERR_NOORIGIN,
	       GETLANG(irc2_ERR_NOORIGIN));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_PRIVMSG
/* handlePRIVMSG
 * Original 01/09/2001 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handlePRIVMSG)
{
   doMessage(parameters, false);
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_QUIT
/* handleQUIT
 * Original 14/08/2001 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleQUIT)
{
   // Make the user quit (we can't help but ignore the error values)
   if (parameters.empty()) {
# ifdef KINE_DEBUG_ASSERT
      assert(user.quit() == Error::NO_ERROR);
# else
      (void)user.quit();
# endif
      if (true) {
	 /* Say goodbye to the client. RFC1459 makes us send an ERROR, despite
	  * it being somewhat of a misnomer!
	  */	
         sendError(GETLANG(irc2user_ERROR_CLOSING_LINK_QUIT,
			   user.getNickname(),
			   user.getUsername(),
			   user.getHostname()));
      }
   } else {
# ifdef KINE_DEBUG_ASSERT
      assert(user.quit(localiseStr(parameters[0])) == Error::NO_ERROR);
# else
      (void)user.quit(localiseStr(parameters[0]));
# endif
      if (true) {
	 // Likewise with above, however including the reason the user gave
	 sendError(GETLANG(irc2user_ERROR_CLOSING_LINK_QUIT_WITH_REASON,
			   user.getNickname(),
			   user.getUsername(),
			   user.getHostname(),
			   localiseStr(parameters[0])));
      }
   }
   
   // Close the connection
   connection.goodbye();
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_REHASH
/* handleREHASH
 * Original 19/09/2001 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleREHASH)
{ 
   static const char* const commandName = "REHASH";
   
   // Try to rehash
   const Error::error_type error = myServer().rehash(user);
   
   // Check the status of the operation
   if (error == Error::NO_ERROR) {
      /* Tell the user we are rehashing.. Technically, we have rehashed! Even
       * if we told the user we were rehashing *before* we started rehashing,
       * the output would not normally not be sent to the user until after the
       * event anyway..
       */
      sendNumeric(LibIRC2::Numerics::RPL_REHASHING,
		  GETLANG(irc2_RPL_REHASHING));
      return;
   }

   // Process the error..
   if (error == Error::PERMISSION_DENIED) {
      // Complain about not having access to this command
      sendNumeric(LibIRC2::Numerics::ERR_NOPRIVILEGES,
		  commandName,
		  GETLANG(irc2_ERR_NOPRIVILEGES_SPECIFIC));
      return;
   }
   
# ifdef KINE_DEBUG_ASSERT
   // Make sure we're sane..
   assert(error != Error::UNREGISTERED_ENTITY);
# endif

   // No idea what the error is
   sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNERROR,
	       commandName,
	       GETLANG(irc2_ERR_UNKNOWNERROR));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_RESTART
/* handleRESTART
 * Original 28/10/2001 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleRESTART)
{
   static const char* const commandName = "RESTART";
   
   // Try to restart the server
   const Error::error_type error =
     myServer().restart(user, localiseStr(parameters[0]));
   
   // Check the status of the operation
   if (error != Error::NO_ERROR) {
      // What's the error?
      if (error == Error::PERMISSION_DENIED) {
	 // Complain about not having access to this command
	 sendNumeric(LibIRC2::Numerics::ERR_NOPRIVILEGES,
		     commandName,
		     GETLANG(irc2_ERR_NOPRIVILEGES_SPECIFIC));
	 return;
      } else if (error == Error::TEXT_TOO_SHORT) {
	 // Complain about the reason being too short..
	 sendNumeric(LibIRC2::Numerics::ERR_TEXTTOOSHORT,
		     commandName,
		     GETLANG(irc2_ERR_TEXTTOOSHORT));
	 return;
      }
	
# ifdef KINE_DEBUG_ASSERT
      // Make sure we're sane..
      assert(error != Error::UNREGISTERED_ENTITY);
# endif

      // Unknown error
      sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNERROR,
		  commandName,
		  GETLANG(irc2_ERR_UNKNOWNERROR));
   }
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_SERVLIST
/* handleSERVLIST
 * Original 30/04/2003 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleSERVLIST)
{
   // Our mask for service names to match
   StringMask nameMask;
   
   // The type bitmask to match..
   long type = 0;
   
   // Were we given a specific mask to match?
   if (parameters.size() >= 1) {
      nameMask = parameters[0];

      // Were we provided a type bitmask?
      if (parameters.size() >= 2) {
	 // Convert the type integer across
	 type = std::atol(parameters[1].c_str());
      }
   } else {
      // Match everything
      nameMask = "*";
   }
   
# warning "The registry can no longer be iterated over!"
   // Iterate over the list of services
   for (Registry::services_type::const_iterator it = 
	registry().getServices().begin();
	it != registry().getServices().end();
	++it) {
      // Does this server match the name mask and the type bitmask?
      if (nameMask.matches(it->second->getNickname().toLower()) &&
	  ((type & it->second->getServiceType()) ||
	   (it->second->getServiceType() == 0))) {
	 // Tell the client about this service
	 sendNumeric(LibIRC2::Numerics::RPL_SERVLIST,
		     it->second->makeUserHostIdent(),
		     it->second->getServer().getName(),
		     it->second->getScopeMask(),
		     it->second->getServiceType(),
		     it->second->getHopCount(),
		     it->second->getDescription());
      }
   }
   
   // Send the end of services list numeric
   sendNumeric(LibIRC2::Numerics::RPL_SERVLISTEND,
	       nameMask,
	       type,
	       GETLANG(irc2_RPL_SERVLISTEND));
}
#endif
  
  
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_SQUERY
/* handleSQUERY
 * Original 04/05/2003 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleSQUERY)
{
   static const char* const commandName = "SQUERY";
   
   // Make sure the command includes the required number of parameters
   if (parameters.size() >= 2) {
      // Tokenise the recipient list
      StringTokens recipients(parameters[0]);
      
      // The number of targets left
      unsigned int targets = config().getLimitsMaxTargets();
      
      // Iterate over the parameters...
      while (recipients.hasMoreTokens()) {
	 std::string target = recipients.nextToken(',');
	 
	 // Make sure this won't be too many targets for this user
	 if ((--targets > 0) || user.isOperator()) {
	    // The directivity (how acutely the target was specified)
	    Receiver::Directivity directivity;
	    
	    // Find something of this name
	    Receiver* const receiver =
	      LibIRC2::Utility::findMessageTarget(localiseStr(target),
						  directivity, true);
	    
	    // Did we find something?
	    if (receiver != 0) {
	       // Try to down-cast this to a service
	       Service* const service =
		 dynamic_cast<Service* const>(receiver);
	       
	       // Is this really a service?
	       if (service != 0) {
		  // Pass the query onto the service
		  Error::error_type error;
		  if ((error = service->sendQuery(user,
						  localiseStr(parameters[1]),
						  directivity)) ==
		      Error::NO_ERROR) {
		     // All is well, next target
		     continue;
		  }
		  
		  /* The service didn't accept the message for some reason.
		   * For now, we don't care what the reason is, other than the
		   * message was obviously not sent..
		   */
		  sendNumeric(LibIRC2::Numerics::ERR_CANNOTSENDTONICK,
			      delocaliseStr(service->getName()),
			      GETLANG(irc2_ERR_CANNOTSENDTONICK_UNRECEPTIVE));
		  continue;
	       }
	       
	       // If we got here, the target was found but it was not a service
	       sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVICE,
			   target,
			   GETLANG(irc2_ERR_NOSUCHSERVICE_NOT_SERVICE));
	       continue;
	    }
	    
	    // If we got here, the target was not found
	    sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVICE,
			target,
			GETLANG(irc2_ERR_NOSUCHSERVICE));
	    continue;
	 }

	 // Too many targets - break the loop
	 sendNumeric(LibIRC2::Numerics::ERR_TOOMANYTARGETS,
		     GETLANG(irc2_ERR_TOOMANYTARGETS,
			     Languages::toWideStr(String::convert(config().
								  getLimitsMaxTargets()))));
	 return;
      }
      
      // The end!
      return;
   }
   
   // Okay, what was missing that we need to complain about?
   if (parameters.empty()) {
      // The recipient was missing, complain about that
      sendNumeric(LibIRC2::Numerics::ERR_NORECIPIENT,
		  GETLANG(irc2_ERR_NORECIPIENT,
			  Languages::toWideStr(commandName)));
   } else {
      // Okay, the text was missing, complain about that then
      sendNumeric(LibIRC2::Numerics::ERR_NOTEXTTOSEND,
		  GETLANG(irc2_ERR_NOTEXTTOSEND));
   }
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_STATS
/* handleSTATS
 * Original 14/08/2001 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleSTATS)
{
   // If there is no server parameter, we have to reply
   if (parameters.size() < 2) {
      /* We may have a specific or over-rided method to handle this and we
       * should check. One perfect example is stats 'm' - libkineircd_irc2 is
       * unable to tell the user how many times commands within mod_irc2user
       * have been used, so we are obviously the only ones who can respond.
       * 
       * The 'm' command is the only additional one we know about, so this ugly
       * and not very flexible code will do for now.
       */
      if (!strncmp(static_cast<const String&>(parameters[0]).toLower().data(),
		   "messages",
		   std::max(parameters[0].length(),
			    (std::string::size_type)1))) {
	 // Iterate over the commands and output their call counters
	 for (Commands::commandList_type::const_iterator it =
	      commands().getCommandList().begin();
	      it != commands().getCommandList().end();
	      ++it) {
	    // Make sure this command has been used (as per RFC1459)
	    if (it->second.callCount > 0) {
	       // Send information about this command
	       sendNumeric(LibIRC2::Numerics::RPL_STATSCOMMANDS,
			   it->first,
			   it->second.callCount);
	    }
	 }
	 
	 // Send the footer
	 sendNumeric(LibIRC2::Numerics::RPL_ENDOFSTATS,
		     parameters[0],
		     GETLANG(irc2_RPL_ENDOFSTATS));
	 return;
      }

      // We don't know about it, maybe the library knows it (a generic query)
      doSTATS(user, parameters[0]);
      return;
   }
   
   // Look up the server?
   const std::string& serverName = parameters[1];
   
   // We didn't find the server..
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVER,
	       serverName,
	       GETLANG(irc2_ERR_NOSUCHSERVER));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_SUMMON
/* handleSUMMON
 * Original 10/04/2003 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleSUMMON)
{
   // If there is no server parameter, we have to reply
   if (parameters.size() < 2) {
      doSUMMON(user, parameters[0]);
      return;
   }
   
   // Look up the server?
   const std::string& serverName = parameters[1];
   
   // We didn't find the server..
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVER,
	       serverName,
	       GETLANG(irc2_ERR_NOSUCHSERVER));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_TIME
/* handleTIME
 * Original 27/08/2001 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleTIME)
{
   // If there are no parameters, the user was wants us to reply..
   if (parameters.empty()) {
      doTIME(user);
      return;
   }
   
   // Look up the server?
   const std::string& serverName = parameters[0];
   
   // We didn't find the server..
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVER,
	       serverName,
	       GETLANG(irc2_ERR_NOSUCHSERVER));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_USERS
/* handleUSERS
 * Original 10/04/2003 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleUSERS)
{
   // If there is no server parameter (no params at all), we have to reply
   if (parameters.empty()) {
      doUSERS(user);
      return;
   }
   
   // Look up the server?
   const std::string& serverName = parameters[0];
   
   // We didn't find the server..
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVER,
	       serverName,
	       GETLANG(irc2_ERR_NOSUCHSERVER));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_USERHOST
/* handleUSERHOST
 * Original 21/08/2001 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleUSERHOST)
{
   unsigned int count = 0;
   std::ostringstream output;
   
   // Run through the parameters
   for (parameters_type::const_iterator it = parameters.begin();
	it != parameters.end();
	++it) {
      // Make sure we have not overstepped the maximum parameter count
      if (++count > 5/* config? */) {
	 break;
      }
      
      // Try to find this user
      const Kine::User* const foundUser =
	myNetwork().findUser(localiseStr(*it));
      
      // Did we find this user?
      if (foundUser != 0) {
	 // Prepend a space delimeter if necessary
	 if (!output.str().empty()) {
	    output << ' ';
	 }
	 
	 // Output the nickname..
	 output << delocaliseStr(foundUser->getName());
	 
	 // If the user is an operator, add a '*' to the end of their nickname
	 if (foundUser->isOperator()) {
	    output << replacementCharacter;
	 }
	 
	 // Output the rest
	 output << '=' <<
	   (foundUser->isAway() ? '-' : '?') <<
	   delocaliseStr(foundUser->getUsername()) << '@' <<
	   delocaliseStr(foundUser->hideHostFrom(user) ? 
			 foundUser->getVirtualHostname() :
			 foundUser->getHostname());
      }
   }
   
   // Send the reply to the user
   sendNumeric(LibIRC2::Numerics::RPL_USERHOST,
	       output.str());
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_USERIP
/* handleUSERIP
 * Original 21/08/2001 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleUSERIP)
{
   unsigned int count = 0;
   std::ostringstream output;
   
   // Run through the parameters
   for (parameters_type::const_iterator it = parameters.begin();
	it != parameters.end();
	++it) {
      // Make sure we have not overstepped the maximum parameter count
      if (++count > 5/* config? */) {
	 break;
      }
      
      // Try to find this user
      const Kine::User* const foundUser = myNetwork().findUser(*it);
      
      // Did we find this user?
      if (foundUser != 0) {
	 // Prepend a space delimeter if necessary
	 if (!output.str().empty()) {
	    output << ' ';
	 }
	 
	 // Output the nickname..
	 output << foundUser->getName();
	 
	 // If the user is an operator, add a '*' to the end of their nickname
	 if (foundUser->isOperator()) {
	    output << replacementCharacter;
	 }
	 
	 // Output the rest
	 output << '=' <<
	   (foundUser->isAway() ? '-' : '?') <<
	   foundUser->getUsername() << '@' <<
	   (foundUser->hideHostFrom(user) ? 
	    foundUser->getVirtualNetworkAddress() :
	    foundUser->getNetworkAddress());
      }
   }
   
   // Send the reply to the user
   sendNumeric(LibIRC2::Numerics::RPL_USERHOST,
	       output.str());
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_VERSION
/* handleVERSION
 * Original 24/08/2001 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleVERSION)
{
   // If there are no parameters, the user was wants us to reply..
   if (parameters.empty()) {
      doVERSION(user);
      sendISUPPORT();
      return;
   }
   
   // Look up the server?
   const std::string& serverName = parameters[0];
   
   // We didn't find the server..
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVER,
	       serverName,
	       GETLANG(irc2_ERR_NOSUCHSERVER));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_WALLOPS
/* handleWALLOPS
 * Original 18/09/2001 simonb
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleWALLOPS)
{
   static const char* const commandName = "WALLOPS";
   
   // Try and send the wallops message
   const Error::error_type error =
     myServer().sendWallops(user, localiseStr(parameters[0]));
   
   // Check the status of the operation
   if (error != Error::NO_ERROR) {
      // What's the error?
      if (error == Error::PERMISSION_DENIED) {
	 // Complain about not having access to this command
	 sendNumeric(LibIRC2::Numerics::ERR_NOPRIVILEGES,
		     commandName,
		     GETLANG(irc2_ERR_NOPRIVILEGES_SPECIFIC));
	 return;
      } else if (error == Error::TEXT_TOO_SHORT) {
	 // Complain about the reason being too short..
	 sendNumeric(LibIRC2::Numerics::ERR_TEXTTOOSHORT,
		     commandName,
		     GETLANG(irc2_ERR_TEXTTOOSHORT));
	 return;
      }
	
# ifdef KINE_DEBUG_ASSERT
      // Make sure we're sane..
      assert(error != Error::UNREGISTERED_ENTITY);
# endif

      // Unknown error
      sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNERROR,
		  commandName,
		  GETLANG(irc2_ERR_UNKNOWNERROR));
   }
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_WHO
/* handleWHO
 * Original 22/08/2001 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleWHO)
{
   static const char* const commandName = "WHO";
   
   // Mask of users to match
   StringMask mask;
   
   // Stuff that determines how the output is filtered
   union {
      struct {
	 bool operators:1;			// Match IRC operators only
      };
      
      unsigned int conflux;
   } filterFlags;
   filterFlags.conflux = 0;
   
   // Determine which fields are supposed to be checked for matches
   union {
      struct {
	 bool address:1;			// Match the IP address
	 bool description:1;			// Match the 'real name' field
	 bool hostname:1;			// Match the user host
	 bool nick:1;				// Match the nickname
	 bool server:1;				// Match the server's name
	 bool username:1;			// Match the user's name
      };
      
      unsigned int conflux;
   } matchFlags;
   matchFlags.conflux = 0;
   
   // These determine what gets sent to the user (which fields they want)
   union {
      struct {
	 bool address:1;			// The IP address
	 bool channelName:1;			// (First) channel's name
	 bool description:1;			// The user's description field
	 bool flags:1;				// All of the user's flags
	 bool hops:1;				// The distance (hop count)
	 bool hostname:1;			// The hostname
	 bool idleTime:1;			// The idle time (if its local)
	 bool nickname:1;			// The nickname
	 bool queryType:1;			// The query type in the reply
	 bool server:1;				// The server name
	 bool username:1;			// The user name (maybe with ~)
      };
      
      unsigned int conflux;
   } outputFlags;
   outputFlags.conflux = 0;
   
   // The 'query type' field (which may be over-written later)
   std::string queryType(replacementParameter);
   
   // Were we given a mask?
   if (parameters.size() >= 1) {
      // Copy the given mask over
      mask = parameters[0];
      
      // Options?
      if (parameters.size() >= 2) {
	 std::string::size_type pos = (std::string::size_type)-1;
	 
	 // Run over the options parameter, and expect filter flags
	 while ((++pos < parameters[1].length()) &&
		(parameters[1][pos] != '%') &&
		(parameters[1][pos] != ',')) {
	    switch (parameters[1][pos]) {
	     // Match the hostname field
	     case 'h':
	     case 'H':
	       matchFlags.hostname = true;
	       continue;
	       
	     // Match the address field
	     case 'i':
	     case 'I':
	       matchFlags.address = true;
	       continue;
	       
	     // Match the nickname field
	     case 'n':
	     case 'N':
	       matchFlags.nick = true;
	       continue;

	     // Filter out IRC operators
	     case 'o':
	     case 'O':
	       filterFlags.operators = true;
	       continue;
	       
	     // Match the description field
	     case 'r':
	     case 'R':
	       matchFlags.description = true;
	       continue;
	       
	     // Match the server name field
	     case 's':
	     case 'S':
	       matchFlags.server = true;
	       continue;

	     // Match the username field
	     case 'u':
	     case 'U':
	       matchFlags.username = true;
	       continue;
	    }
	 }
	 
	 // Do we need to process any output flags?
	 if (parameters[1][pos] == '%') {
	    while ((++pos < parameters[1].length()) &&
		   (parameters[1][pos] != ',')) {
	       switch (parameters[1][pos]) {
		// Output the channel name?
		case 'c':
		case 'C':
		  outputFlags.channelName = true;
		  continue;
		  
		// Output the hop count?
		case 'd':
		case 'D':
		  outputFlags.hops = true;
		  continue;
		  
		// Output the flags?
		case 'f':
		case 'F':
		  outputFlags.flags = true;
		  continue;
		  
		// Output the hostname?
		case 'h':
		case 'H':
		  outputFlags.hostname = true;
		  continue;
		  
		// Output the IP address?
		case 'i':
		case 'I':
		  outputFlags.address = true;
		  continue;
		  
		// Output the user's idle time?
		case 'l':
		case 'L':
		  outputFlags.idleTime = true;
		  continue;
		  
		// Output the nickname?
		case 'n':
		case 'N':
		  outputFlags.nickname = true;
		  continue;
		  
		// Output the description field?
		case 'r':
		case 'R':
		  outputFlags.description = true;
		  continue;
		  
		// Output the server name?
		case 's':
		case 'S':
		  outputFlags.server = true;
		  continue;
		  
		// Output the given query type field?
		case 't':
		case 'T':
		  outputFlags.queryType = true;
		  continue;
		  
		// Output the user name?
		case 'u':
		case 'U':
		  outputFlags.username = true;
		  continue;
	       }
	    }
	 }
	 
	 // Do we have a query type field?
	 if ((parameters[1][pos] == ',') &&
	     (++pos < parameters[1].length())) {
	    queryType = parameters[1].substr(pos);
	 }
      }
   } else {
      // Match everything we can.. *sigh*...
      mask = "*";
   }

   // If we were not given any fields to match, match them all (*sigh*)
   if (matchFlags.conflux == 0) {
      matchFlags.conflux = (unsigned int)-1;
   }

   // The number of lines left before we truncate the output
   unsigned int linesLeft = 100; // <=- configuration variable?!
   
# warning "The registry can no longer be iterated over!"
   // Run over the user list
   for (Registry::users_type::const_iterator it = registry().getUsers().begin();
	it != registry().getUsers().end();
	++it) {
      // Does this user match? (ick, this logic is pretty ugly..)
      if ((mask != "0") &&
	  !((matchFlags.address &&
	     false) ||
	    (matchFlags.description &&
	     mask.matches(it->second->getDescription())) ||
	    (matchFlags.hostname &&
	     (it->second->hideHostFrom(user) ?
	      mask.matches(it->second->getVirtualHostname()) :
	      (mask.matches(it->second->getHostname()) ||
	       mask.matches(it->second->getVirtualHostname())))) ||
	    (matchFlags.nick &&
	     mask.matches(it->second->getNickname())) ||
	    (matchFlags.server &&
	     false) ||
	    (matchFlags.username &&
	     mask.matches(it->second->getUsername())))) {
	 // After all that, there was no match..
	 continue;
      }
      
      // Okay, if we got here the user matches.. Make sure the user can be seen
      if (it->second->isHiddenFrom(user)) {
	 continue;
      }
      
      // Check this user against our filters
      if (filterFlags.operators && !it->second->isOperator()) {
	 continue;
      }
      
      // Stuff we (may) need
      const Channel* firstVisibleChannel = 0;
      std::string flags;
      
      // Do we need to assemble flags?
      if ((outputFlags.conflux == 0) || outputFlags.flags) {
	 // The user flags start with whether or not the user is here or gone
	 flags += (it->second->isAway() ? 'G' : 'H');
	 
	 // If the user is an IRC operator, they get an asterick (lucky them!)
	 if (it->second->isOperator()) {
	    flags += '*';
	 }
	 
	 // If we found a channel, check their membership status for anything..
	 if (firstVisibleChannel != 0) {
	    // Find the user's prominent membership status char (if any)
	    // something.
	 }
	 
	 // If the user is 'deaf' (they have selective hearing), add a 'd'
	 if (it->second->hasSelectiveHearing()) {
	    flags += 'd';
	 }
      }
      
      // Work out if we are giving the user a special output or a 'normal' one
      if (outputFlags.conflux == 0) {
	 // Nothing special, return a normal reply a la RFC1459
	 sendNumeric(LibIRC2::Numerics::RPL_WHOREPLY,
		     ((firstVisibleChannel != 0) ?
		      firstVisibleChannel->getChannelName() :
		      replacementParameter),
		     it->second->getUsername(),
		     (it->second->hideHostFrom(user) ?
		      it->second->getVirtualHostname() :
		      it->second->getHostname()),
		     "server.name",
		     it->second->getNickname(),
		     flags,
		     String::convert(it->second->getHopCount()) + ' ' +
		     it->second->getDescription());
      } else {
	 // We must assemble the output specially, in a predetermined order..
	 std::ostringstream output;
	 
	 // Did the user want the query type field?
	 if (outputFlags.queryType) {
	    output << queryType;
	 }
	 
	 // Did the user want the channel name field?
	 if (outputFlags.channelName) {
	    if (!output.str().empty()) {
	       output << ' ';
	    }
	    output << ((firstVisibleChannel != 0) ?
		       firstVisibleChannel->getChannelName() :
		       replacementParameter);
	 }
	 
	 // Did the user want the 'username' field?
	 if (outputFlags.username) {
	    if (!output.str().empty()) {
	       output << ' ';
	    }
	    output << it->second->getUsername();
	 }
	 
	 // Did the user want the 'address' field
	 if (outputFlags.address) {
	    if (!output.str().empty()) {
	       output << ' ';
	    }
	    output << replacementCharacter; // fix me
	 }
	 
	 // Did the user want the 'hostname' field?
 	 if (outputFlags.hostname) {
	    if (!output.str().empty()) {
	       output << ' ';
	    }
	    output << (it->second->hideHostFrom(user) ?
		       it->second->getVirtualHostname() :
		       it->second->getHostname());
	 }
	 
	 // Did the user want the server name field?
	 if (outputFlags.server) {
	    if (!output.str().empty()) {
	       output << ' ';
	    }
	    output << "server.name"; // fix me
	 }
	 
	 // Did the user want the nickname field?
	 if (outputFlags.nickname) {
	    if (!output.str().empty()) {
	       output << ' ';
	    }
	    output << it->second->getNickname();
	 }
	 
	 // Did the user want the flags?
	 if (outputFlags.flags) {
	    if (!output.str().empty()) {
	       output << ' ';
	    }
	    output << flags;
	 }
	 
	 // Did the user want the hop count?
	 if (outputFlags.hops) {
	    if (!output.str().empty()) {
	       output << ' ';
	    }
	    output << it->second->getHopCount();
	 }
	 
	 // Did the user want the user's idle time?
	 if (outputFlags.idleTime) {
	    if (!output.str().empty()) {
	       output << ' ';
	    }
	    
	    // Try to convert this over to a local user (should use typeid()?)
	    const LocalUser* const lu =
	      dynamic_cast<const LocalUser* const>(it->second);
	    
	    // Well, was it a local user?
	    if (lu != 0) {
	       output << (daemon().getTime() - lu->getLastAwake()).seconds;
	    } else {
	       // We cannot determine the idle time of this user
	       output << 0;
	    }
	 }
	 
	 // Did the user want the 'real name' field?
	 if (outputFlags.description) {
	    if (!output.str().empty()) {
	       output << ' ';
	    }
	    output << ':' << it->second->getDescription();
	 }
	 
	 /* Send the info using a 'special' form (ick, will this break any
	  * portability issues??)
	  */
	 sendNumericRaw(LibIRC2::Numerics::RPL_WHOSPCRPL,
			output.str());
      }
      
      // Is the user is not allowed to match endlessly?
      if (!user.isOperator()) {
	 // Decrease and check the lines left counter..
	 if (--linesLeft == 0) {
	    // Break, so the end numeric can be sent for old clients
	    break;
	 }
      }
   }
   
   // Send the end of who numeric
   sendNumeric(LibIRC2::Numerics::RPL_ENDOFWHO,
	       mask,
	       GETLANG(irc2_RPL_ENDOFWHO));

   // If the output ended because it was truncated, say so now
   if (linesLeft == 0) {
      sendNumeric(LibIRC2::Numerics::ERR_TOOMANYMATCHES,
		  commandName,
		  mask,
		  GETLANG(irc2_ERR_TOOMANYMATCHES));
   }
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_WHOIS
/* handleWHOIS
 * Original 23/08/2001 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleWHOIS)
{
   // If there is only one parameter, we are the intended respondent
   if (parameters.size() < 2) {
      doWHOIS(user, parameters[0]);
      return;
   }
   
   // Look up the server?
   const std::string& serverName = parameters[0];
   
   // We didn't find the server..
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVER,
	       serverName,
	       GETLANG(irc2_ERR_NOSUCHSERVER));
}
#endif


#ifdef KINE_MOD_IRC2USER_HAVE_CMD_WHOWAS
/* handleWHOWAS
 * Original 09/10/2001 simonb
 * Note: Incomplete
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleWHOWAS)
{
   // If there is only or two parameters, we are the intended respondent
   if (parameters.size() < 3) {
      // Were we given the 'count' parameter?
      if (parameters.size() == 2) {
	 // Do the WHOWAS with the count parameter
	 doWHOWAS(user, parameters[0], parameters[1].toInt());
	 return;
      } 

      // Do the WHOWAS without the count parameter
      doWHOWAS(user, parameters[0]);
      return;
   }
   
   // Look up the server?
   const std::string& serverName = parameters[0];
   
   // We didn't find the server..
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVER,
	       serverName,
	       GETLANG(irc2_ERR_NOSUCHSERVER));
}
#endif
