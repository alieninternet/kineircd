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
#include <sstream>
#include <iomanip>
#include <aisutil/string.h>

#include "mod_irc2user/protocol.h"
#include "mod_irc2user/lang.h"
#include "mod_irc2user/commands.h"

using namespace Kine::mod_irc2user;
using AISutil::String;
using AISutil::StringTokens;


/* handleADMIN
 * Original 27/08/2001 simonb
 * 10/04/2003 simonb - Imported from old code (incomplete)
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleADMIN)
{
   // If there are no parameters, the user was wants us to reply..
   if (parameters.empty()) {
      doADMIN(user);
      return;
   }
   
   // Look up the server?
   const std::string& serverName = parameters[0];
   
   // We didn't find the server..
   sendNumeric(LibIRC2::Numerics::ERR_NOSUCHSERVER,
	       serverName,
	       GETLANG(irc2_ERR_NOSUCHSERVER));
}


/* handleAWAY
 * Original 13/08/2001 simonb
 * 18/04/2003 simonb - Imported from old code (incomplete)
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleAWAY)
{
   static const char* const commandName = "AWAY";
   
   // If there is a parameter, the user is going away
   if (!parameters.empty()) {
      // Set the user 'away'
      if (user.setAway(parameters[0]) == Error::NO_ERROR) {
	 return;
      }
   } else {
      // No parameters were given, so the user is now here..
      if (user.setHere() == Error::NO_ERROR) {
	 return;
      }
   }

   // What happened?? No idea..
   sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNERROR,
	       commandName,
	       GETLANG(irc2_ERR_UNKNOWNERROR));
}


/* handleHELP
 * Original 13/08/2001 simonb
 * 02/04/2003 simonb - Imported from old code
 * Note: Not the best use on CPU since it's checking a match no matter how
 *       this is called, but since this is very rarely called I decided the
 *       code should be small rather than faster :)
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleHELP)
{
   static const char* const commandName = "HELP";
   
   bool extended = false;
   AISutil::StringMask mask;
   
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
   
   // Fix the mask up so it matches properly
   mask = mask.toUpper();
   
   // Run through the list of functions
   for (Commands::commandList_type::const_iterator it =
	Commands::getInstance().getCommandList().begin();
	it != Commands::getInstance().getCommandList().end();
	++it)
     {
	/* Check if this is the command our user was looking for, whether the
	 * command has help, and whether the user even has access to this
	 * command in the first place!
	 */
	if (mask.matches((*it).first) &&
	    ((*it).second.helpUsage != 0) &&
	    (*it).second.hasAccess(user)) {
	   // Send the user the usage help for this function, if we can
	   if (*((*it).second.helpUsage) != 0) {
	      sendNumeric(LibIRC2::Numerics::RPL_HELP,
			  (*it).first,
			  GETLANG_BY_ID(*((*it).second.helpUsage)));
	   }
	   
	   // If we are doing extended help, send the extended lines
	   if (extended &&
	       ((*it).second.helpInfo != 0)) {
	      // Grab the extended help string..
	      const std::string help = GETLANG_BY_ID(*((*it).second.helpInfo));
	      
	      // Our starting and ending positions..
	      std::string::size_type startPosition = 0;
	      std::string::size_type endPosition;
	      
	      // The max number of chars we can send.. (is this algo okay??)
	      const std::string::size_type maxChars =
		maxMessageSize -
		Kine::config().getOptionsServerName().length() -
		user.getNickname().length() -
		25;
	      
	      // Loop until we have no more text left..
	      for (;;) {
		 // How much do we need to break this line up?
		 if ((help.length() - startPosition) > maxChars) {
		    // Work out where we can break this line (at a space)
		    endPosition = help.rfind(' ',
					     (startPosition + maxChars));
		 } else {
		    // Set the end position to the end of the line!
		    endPosition = help.length();
		 }
		 
		 // Send this bit..
		 sendNumeric(LibIRC2::Numerics::RPL_MOREHELP,
			     (*it).first,
			     help.substr(startPosition,
					 (endPosition - startPosition)));
		 
		 // If we're now at the end of the line, break out of this loop
		 if (endPosition == help.length()) {
		    break;
		 }
		 
		 // If we're still going, remember where we last broke..
		 startPosition = endPosition + 1;
	      }
	   }
	}
     }
   
   // Say bye bye
   sendNumeric(LibIRC2::Numerics::RPL_ENDOF_GENERIC,
	       commandName, mask,
	       GETLANG(irc2_RPL_ENDOF_GENERIC_HELP));
}


/* handleLANGUAGE
 * Original 26/10/2001 simonb
 * 03/04/2003 simonb - Imported from old code
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
	 // Make the maintainer string
	 std::string maintainer(it->second->getMaintainer());
	 
	 // Fill the maintainer string if it is empty
	 if (maintainer.empty()) {
	    maintainer = '*';
	 }
	 
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
	    modes = '*';
	 }
	 
	 // Do we send the language with or without a note?
	 if (it->second->getLanguageNote().empty()) {
	    sendNumeric(LibIRC2::Numerics::RPL_LANGUAGE,
			it->second->getLanguageCode(),
			it->second->getFileRevision(),
			maintainer,
			modes,
			it->second->getLanguageName());
	 } else {
	    // Put together the name field
	    std::ostringstream nameField;
	    nameField <<
	      it->second->getLanguageName() << " (" <<
	      it->second->getLanguageNote() << ')';
	    sendNumeric(LibIRC2::Numerics::RPL_LANGUAGE,
			it->second->getLanguageCode(),
			it->second->getFileRevision(),
			maintainer,
			modes,
			nameField.str());
	 }
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
   if (user.setLanguageList(languageList) == Error::NO_ERROR) {
      return;
   }
   
   // Errm, this is bad..
   sendNumeric(LibIRC2::Numerics::ERR_UNKNOWNERROR,
	       commandName,
	       GETLANG(irc2_ERR_UNKNOWNERROR));
}


/* handleLUSERS
 * Original 27/08/2001 simonb
 * 03/04/2003 simonb - Imported from old code (incompleted)
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


/* handleMOTD
 * Original 13/08/2001 simonb
 * 03/04/2003 simonb - Imported from old code (incompleted)
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


/* handlePING
 * Original 25/08/2001 simonb
 * 03/04/2003 simonb - Imported from old code
 */
IRC2USER_COMMAND_HANDLER(Protocol::handlePING)
{
   // If we were given a parameter, send it back (simple as that!)
   if (!parameters.empty()) {
      // Send the reply (pong) in full form
      sendMessageFrom(config().getOptionsServerName(), "PONG",
		      config().getOptionsServerName(), 
		      parameters[0]);
      return;
   }
   
   // Umm, return a complaint as per standards, even if it is kinda wrong
   sendNumeric(LibIRC2::Numerics::ERR_NOORIGIN,
	       GETLANG(irc2_ERR_NOORIGIN));
}


/* handleQUIT
 * Original 14/08/2001 simonb
 * 08/04/2003 simonb - Imported from old code (incompleted)
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleQUIT)
{
   // Let the core know about the client quitting
   
   // Say goodbye to the client
   
   // Close the connection
   connection.goodbye();
}


/* handleSUMMON
 * Original 10/04/2003 simonb
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


/* handleTIME
 * Original 27/08/2001 simonb
 * 14/04/2003 simonb - Imported from old code (incompleted; needs config stuff)
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


/* handleUSERS
 * Original 10/04/2003 simonb
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


/* handleVERSION
 * Original 24/08/2001 simonb
 * 03/04/2003 simonb - Imported from old code (incompleted)
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


/* handleWHOIS
 * Original 23/08/2001 simonb
 * 20/04/2003 simonb - Imported from old code (incompleted)
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
