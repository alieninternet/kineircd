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
#ifdef KINE_DEBUG_ASSERT
# include <cassert>
#endif
#include <kineircd/config.h>
#include <kineircd/languages.h>
#include <kineircd/registry.h>
#include <kineircd/version.h>
#include <aisutil/string/string.h>

#include "mod_irc2user/protocol.h"
#include "mod_irc2user/lang.h"
#include "mod_irc2user/commands.h"
#include "mod_irc2user/isupport.h"

using namespace Kine::mod_irc2user;
using AISutil::String;


// Our "official" protocol name
const char* const Protocol::protocolName =
  "irc2user";


/* Protocol - Constructor/Initialise the connection
 * Original 12/08/2001 simonb
 */
Protocol::Protocol(const Kine::Registrant& registrant,
		   Kine::Connection& connection, std::string& inputQueue,
		   std::string& outputQueue)
  : Kine::LibIRC2::Protocol(connection, inputQueue, outputQueue),
    user(*this, registrant, connection.getSocket().getRemoteAddress()),
    maxMessageSize(512) // <=- should really be configurable
{
   // Attempt to register the user to the registry..
   if (registry().addUser(user) != Error::NO_ERROR) {
      // Deal with this in a nice way.. but we don't, yet, do we?
      return;
   };

   // Cheat. Add language info which doesn't yet get fed from the registrar
   if (user.
       setLanguageList(user,
		       Languages::languageDataList_type(1,
							languages().
							getDefaultLanguage()),
		       true) !=
       Error::NO_ERROR) {
      // Deal with this nicely too..
      return;
   }
   
   // Welcome the user to the server (001)
   sendNumeric(LibIRC2::Numerics::RPL_WELCOME,
	       GETLANG(irc2_RPL_WELCOME,
		       user.getNickname(),
		       user.getUsername(),
		       user.getHostname()));
   
   // Tell the user about ourselves - hostname and version (002)
   sendNumeric(LibIRC2::Numerics::RPL_YOURHOST,
	       GETLANG(irc2_RPL_YOURHOST,
		       config().getServerName(),
		       Kine::Version::version));
   
   // Tell the user when the server was created, not that they'd care (003)
   sendNumeric(LibIRC2::Numerics::RPL_CREATED,
	       GETLANG(irc2_RPL_CREATED,
		       Kine::Version::buildTimeFull));
   
   // Tell the user a little bit about what we know.. (004)
   sendNumeric(LibIRC2::Numerics::RPL_MYINFO,
	       config().getServerName(),
	       Kine::Version::version,
	       "umodes",
	       "cmodes",
	       "cparammodes",
	       "uparammodes",
	       "smodes",
	       "sparammodes");
   
   // Tell the user a about what we can do.. (005 numeric)
   sendISUPPORT();

   // Tell the client what their language is set to..
   sendLanguageList();
   
   // Tell the user the time on server
   sendTimeOnServer(user);
   
   // Do an LUSERS command, for some reason some clients want this??
   doLUSERS(user);
   
   // Also do the MOTD command
   doMOTD(user, true);
}


/* Protocol - Destroy the connection
 * Original 14/08/2001 simonb
 */
Protocol::~Protocol(void)
{
   // Deregister our user, we hope.. It is not smart to ignore the error..
   (void)registry().removeUser(user);
}


/* parseMessage - Appropriately parse a protocol message
 * Original 12/08/2001 simonb
 */
void Protocol::parseMessage(const std::string& origin,
			    const std::string& command,
			    const Kine::LibIRC2::Protocol::parameters_type&
			    parameters)
{
   // Try and find the command..
   const Commands::CommandInfo* const commandInfo = 
     commands().findCommand(command);
   
   // Did we find it? (most of the time we will find it
   if (commandInfo != 0) {
      // Is this command enabled?
      if (commandInfo->enabled) {
	 // Does this user have access to this command?
	 if (commandInfo->hasAccess(user)) {
	    // Do we have enough parameters?
	    if (parameters.size() >= commandInfo->minimumParams) {
	       // Does this command reset the user's idle time?
	       if (commandInfo->flags & Commands::Flags::CHANGE_IDLE_TIME) {
		  user.updateLastAwake();
	       }
	       
#if defined(KINE_MOD_IRC2USER_SCHEME_ENABLED) && defined(KINE_DEBUG_ASSERT)
	       /* If we are doing scheme scripting, then a null handler
		* is the sign of something being very very wrong... Break
		* here to prove a point!
		*/
	       assert(commandInfo->handler != 0);
#endif
	       
#ifdef KINE_MOD_IRC2USER_SCHEME_ENABLED
	       // Is it a built-in command?
	       if (commandInfo->handler != 0) {
#endif
		  // Call it..
		  (this->*(commandInfo->handler))(parameters);
#ifdef KINE_MOD_IRC2USER_SCHEME_ENABLED
		  return;
	       }
	       
	       // Check if this is an alias, or a scriping thingy, or something

	       
	       /* If we get here, there was not enough information about the
		* command for us to do anything useful with it. In such a
		* case, we should act 'dumb' and say we don't know about the
		* command (to the client)..
		*/
	       sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNCOMMAND,
			   command,
			   GETLANG(irc2_ERR_UNKNOWNCOMMAND));
#endif
	       return;
	    }
	    
	    // Complain about not having enough parameters, and return
	    sendNumeric(LibIRC2::Numerics::ERR_NEEDMOREPARAMS,
			command,
			GETLANG(irc2_ERR_NEEDMOREPARAMS));
	    return;
	 }
	 
	 // Complain about not having access to this command
	 sendNumeric(LibIRC2::Numerics::ERR_NOPRIVILEGES,
		     command,
		     GETLANG(irc2_ERR_NOPRIVILEGES));
	 return;
      }
      
      // Complain about the command being disabled...
      sendNumeric(LibIRC2::Numerics::ERR_DISABLED,
		  command,
		  GETLANG(irc2_ERR_DISABLED));
      return;
   }

   // If we got here, the command was not found - tell the user the bad news
   sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNCOMMAND,
	       command,
	       GETLANG(irc2_ERR_UNKNOWNCOMMAND));
}


/* sendISUPPORT - Send ISUPPORT information to the user
 * Original 24/08/2001 simonb
 * Note: This *must* be cached, when the events system is written. It should be
 *       rewritten to store data in a std::map to be more flexible for
 *       parameter number and message length changes, too, but for now this
 *       will do, as currently there are no standard methods for such changes 
 *       to be made.
 */
void Protocol::sendISUPPORT(void)
{
   // Save the suffix string, since we could be using it a few times..
   std::string suffix = GETLANG(irc2_RPL_ISUPPORT);
   
   // Our output buffer
   std::string output;

   // Number of parameters have left before it is necessary to purge the buffer
   unsigned int params = 0;
   
   // Determine the maximum number of octets we can send (the 10 is 'safe')
   const unsigned int maxOutputLength = 
     maxMessageSize - suffix.length() - user.getNickname().length() -
     config().getServerName().length() - 10;
   
   // Iterate over the information
   for (ISupport::info_type::const_iterator it = isupport().getInfo().begin();
	it != isupport().getInfo().end();
	++it) {
      // Do we need to purge the buffer (too many parameters or too long?)
      if ((++params > (maxParams - 2)) ||
	  ((output.length() + it->length()) > maxOutputLength)) {
	 // Send this chunk
	 sendNumeric(LibIRC2::Numerics::RPL_ISUPPORT,
		     output,
		     suffix);
	 
	 // Reset the buffer and the parameter count
	 output.clear();
	 params = 0;
      }
      
      // Do we need to prefix the output with a space (delimiter)
      if (!output.empty()) {
	 output += ' ';
      }
      
      // Add this tag to the output
      output += *it;
   }
   
   // If there is something in the buffer, send it..
   if (!output.empty()) {
      sendNumeric(LibIRC2::Numerics::RPL_ISUPPORT,
		  output,
		  suffix);
   }
}


/* sendLanguageList - Send the list of languages a user has set to the user
 * Original 26/10/2001 simonb
 */
void Protocol::sendLanguageList(void)
{
   std::ostringstream langs;
   
   // If the user's language list is empty, they have nothing set..
   if (user.getLanguageList().empty()) {
      langs << Languages::nullLanguageCode;
   } else {
      // Iterate over the language list, and copy out the language codes
      for (Languages::languageDataList_type::const_iterator it = 
	   user.getLanguageList().begin();
	   it != user.getLanguageList().end();
	   ++it) {
	 // Prefix this with a comma?
	 if (!langs.str().empty()) {
	    langs << ',';
	 }
	 
	 // If this is a null language, return that name..
	 if (*it == 0) {
	    langs << Languages::nullLanguageCode;
	    continue;
	 }
	 
	 // Output the code
	 langs << (*it)->getLanguageCode();
      }
   }

   // Send the language list to the client
   sendNumeric(LibIRC2::Numerics::RPL_YOURLANGUAGEIS,
	       langs.str(),
	       GETLANG(irc2_RPL_YOURLANGUAGEIS,
		       (user.getLanguageList()[0])->getLanguageName()));
}


/* sendNames - Send RPL_NAMREPLY's for the given channel
 * Original 15/08/2001 simonb
 */
void Protocol::sendNames(const Channel& channel)
{
   // Work out the magic char for this channel..
   const char visibilityChar =
     (channel.isSecret() ? '@' : (channel.isPrivate() ? '*' : '='));
   
   // Determine the maximum number of octets we can send (the 15 is 'safe')
   const unsigned int maxOutputLength = 
     maxMessageSize - user.getNickname().length() -
     config().getServerName().length() - 15;

   // Our reply buffer..
   std::string reply;
   
   // Iterate over the members list
   for (Channel::members_type::const_iterator it =
	channel.getMembers().begin();
	it != channel.getMembers().end();
	++it) {
      // Can this user see this member?
      if ((/*configuration value &&*/ user.isOperator()) ||
	  (/*configuration value &&*/ user.isStaff()) ||
	  channel.hasClient(user) ||
	  (!channel.isHiddenFrom(user) &&
	   !it->second->getClient().isHiddenFrom(user))) {
	 // Is it necessary to purge the buffer and start a fresh?
	 if ((reply.length() + 
	      it->second->getClient().getNickname().length() + 2) >
	     maxOutputLength) {
	    // Output the names we have within this buffer
	    sendNumeric(LibIRC2::Numerics::RPL_NAMREPLY,
			visibilityChar,
			channel.getName(),
			reply);
	    
	    // Clear the output
	    reply.clear();
	 }
	 
	 // Do we need to prefix this with a space?
	 if (!reply.empty()) {
	    reply += ' ';
	 }
	 
	 // Work out the membership status of this member here..
	 // something.

	 // Append the nickname of this member
	 reply += it->second->getClient().getNickname();
      }
   }
   
   // If there is something left to output, output it
   if (!reply.empty()) {
      sendNumeric(LibIRC2::Numerics::RPL_NAMREPLY,
		  visibilityChar,
		  channel.getName(),
		  reply);
   }
}
