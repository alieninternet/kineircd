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
#include <kineircd/config.h>
#include <kineircd/languages.h>
#include <kineircd/registry.h>
#include <kineircd/version.h>
#include <aisutil/string/string.h>

#include "mod_irc2user/protocol.h"
#include "mod_irc2user/lang.h"
#include "mod_irc2user/commands.h"

using namespace Kine::mod_irc2user;
using AISutil::String;


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
   if (registry().add(user) != Error::NO_ERROR) {
      // Deal with this in a nice way.. but we don't, yet, do we?
      return;
   };

   // Cheat. Add language info which doesn't yet get fed from the registrar
   if (user.
       setLanguageList(Languages::languageDataList_type(1,
							langs().
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
		       config().getOptionsServerName(),
		       Kine::Version::versionFull));
   
   // Tell the user when the server was created, not that they'd care (003)
   sendNumeric(LibIRC2::Numerics::RPL_CREATED,
	       GETLANG(irc2_RPL_CREATED,
		       Kine::Version::buildTimeFull));
   
   // Tell the user a little bit about what we know.. (004)
   sendNumeric(LibIRC2::Numerics::RPL_MYINFO,
	       config().getOptionsServerName(),
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
   (void)registry().remove(user);
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
   // Try and find the command..
   const Commands::CommandInfo* const commandInfo =
     Commands::getInstance().findCommand(command);
   
   // Did we find it? (most of the time we will find it
   if (commandInfo != 0) {
      // Is this command enabled?
      if (commandInfo->enabled) {
	 // Does this user have access to this command?
	 if (commandInfo->hasAccess(user)) {
	    // Do we have enough parameters?
	    if (parameters.size() >= commandInfo->minimumParams) {
	       // Is it a built-in command?
	       if (commandInfo->handler != 0) {
		  // Call it..
		  (this->*(commandInfo->handler))(parameters);
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
   std::ostringstream output;

   // Put the ISUPPORT string together
   output <<
     "NICKLEN=" << (int)config().getLimitsUsersMaxNickNameLength() << " "
     "TOPICLEN=" << config().getLimitsChannelsMaxTopicLength() << " "
     "KICKLEN=" << config().getLimitsMaxKickReasonLength() << " "
     "CHANNELLEN=" << config().getLimitsChannelsMaxNameLength() << " "
     "MAXCHANNELS=" << config().getLimitsUsersMaxChannels() << " "
     "MAXBANS=" << config().getLimitsChannelsMaxBans() << " "
     "MAXTARGETS=" << (int)config().getLimitsMaxTargets() << " "
     "PREFIX=(.ohv).@%+ "
     "CHANTYPES=&#!+.~ "
     "CHIDLEN=5 "
     "CHANMODES=beI,k,l,imnpsta "
     "SILENCE=" << config().getLimitsUsersMaxSilences() << " "
     "CALLERID=" << config().getLimitsUsersMaxAccepts();
   
   // Send this chunk (13 params + text = 14 params)
   sendNumeric(LibIRC2::Numerics::RPL_ISUPPORT,
	       output.str(),
	       suffix);
   
   // New output ostringstream (how can we clean/reuse the old one?)
   std::ostringstream output2;
   
   // Next section..
   output2 <<
     "STATUSMSG=+%@ "
     "MODES=" << (int)0 << " "
     "CASEMAPPING=rfc1459 "
     "CHARSET=UTF-8 "
     "FNC "
     "PENALTY "
     "INVEX "
     "EXCEPTS";
   
   // If there's a network name, output that too
   if (!config().getNetworkName().empty()) {
      output2 << " "
	"NETWORK=" << config().getNetworkName();
   }

   // Compile the language information
   output2 << " "
     "LANGUAGE=" << (int)config().getLimitsUsersMaxLanguages();

   for (Languages::languageDataMap_type::const_iterator it = 
	languages().getLanguageDataMap().begin();
	it != languages().getLanguageDataMap().end();
	++it) {
      output2 << ',';
      
      // Is this language the default one?
      if (it->second == languages().getDefaultLanguage()) {
	 output2 << '*';
      }
      
      // Output the code
      output2 << it->second->getLanguageCode();
   }
   
   // Send this chunk (~ 10 parameters)
   sendNumeric(LibIRC2::Numerics::RPL_ISUPPORT,
	       output2.str(),
	       suffix);
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
