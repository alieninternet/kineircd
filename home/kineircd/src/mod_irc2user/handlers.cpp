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
#include <aisutil/string/mask.h>
#include <kineircd/version.h>

#include "mod_irc2user/protocol.h"
#include "mod_irc2user/language.h"
#include "mod_irc2user/commands.h"

using namespace Kine::mod_irc2user;


/* handleHELP
 * Original 13/08/2001 simonb
 * 02/04/2003 simonb - Imported from old code
 * Note: Not the best use on CPU since it's checking a match no matter how
 *       this is called, but since this is very rarely called I decided the
 *       code should be small rather than faster :)
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleHELP)
{
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
   
   // Run through the list of functions (this should be std::for_each<>)
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
	       "HELP", mask,
	       GETLANG(irc2_RPL_ENDOF_GENERIC_HELP));
}


/* handleLANGUAGE
 * Original 26/10/2001 simonb
 * 03/04/2003 simonb - Imported from old code
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleLANGUAGE)
{
   // If we do not have any parameters, we will list the languages available
   if (parameters.empty()) {
      // Run through known languages and list them to the client
      // Hey - this needs to be for_each'd :(
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
	 
	 // If this is the current language, add a 's'
//	 if (it->second == something) {
//	    modes += 's';
//	 }
	 
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
		  "LANGUAGE",
		  GETLANG(irc2_RPL_ENDOF_GENERIC_LANGUAGE));
      return;      
   }
}


/* handleLUSERS
 * Original 27/08/2001 simonb
 * 03/04/2003 simonb - Imported from old code
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleLUSERS)
{
   // If there are no parameters, the user was wants us to reply..
   if (parameters.empty()) {
      sendLUSERS();
      return;
   }
   
   // Look up the server?
}


/* handleMOTD
 * Original 13/08/2001 simonb
 * 03/04/2003 simonb - Imported from old code
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleMOTD)
{
   // If there are no parameters, the user was wants us to reply..
   if (parameters.empty()) {
      sendMOTD();
      return;
   }
   
   // Look up the server?
}


/* handlePING
 * Original 25/08/2001 simonb
 * 03/04/2003 simonb - Imported from old code
 */
IRC2USER_COMMAND_HANDLER(Protocol::handlePING)
{
   // If we were given a parameter, send it back (simple as that!)
   if (!parameters.empty()) {
      // Send the reply in full form
      sendMessageTo(config().getOptionsServerName(), 
		    config().getOptionsServerName(), 
		    "PONG", parameters[0]);
      return;
   }
   
   // Umm, return a complaint as per standards, even if it is technically wrong
   sendNumeric(LibIRC2::Numerics::ERR_NOORIGIN,
	       GETLANG(irc2_ERR_NOORIGIN));
}


/* handleQUIT
 * Original 14/08/2001 simonb
 * 08/04/2003 simonb - Imported from old code
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleQUIT)
{
   // Let the core know about the client quitting
   
   // Say goodbye to the client
   
   // Close the connection
   connection.goodbye();
}


/* handleTIME
 * Original 27/08/2001 simonb
 * 14/04/2003 simonb - Imported from old code
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleTIME)
{
   // If there are no parameters, the user was wants us to reply..
   if (parameters.empty()) {
      // Make up a "human readable" time string
      struct tm currentTime;
      currentTime = *gmtime((const time_t*)&daemon().getTime());
      std::ostringstream text;
      
//      if (true) { // <=- should be a configurable define or something
	 /* Stuff used to make a human readable time string which confirms
	  * relatively strictly to what traditional IRC daemons output.
	  * Unfortunately it is not internationalised, and too many 
	  * scripts/bots rely on it. Hopefully this changes one day.
	  */
	 static const char* const months[12] = {
	    "January", "February", "March", "April", "May", 
	    "June", "July", "August", "September", "October",
	    "November", "December"
	 };
	 static const char* const days[7] = {
	    "Sunday", "Monday", "Tuesday", "Wednesday",
	    "Thursday", "Friday", "Saturday"
	 };
	 
	 text << std::setfill('0') <<
	   days[currentTime.tm_wday] << ' ' <<
	   months[currentTime.tm_mon] << ' ' <<
	   currentTime.tm_mday << ' ' <<
	   (currentTime.tm_year + 1900) << " -- " <<
	   std::setw(2) << currentTime.tm_hour << ':' <<
	   std::setw(2) << currentTime.tm_min << ' ' <<
	   ((timezone > 0) ? '-' : '+') <<
	   std::setw(2) << ((-timezone) / 3600) << ':' <<
	   std::setw(2) << (((-timezone) % 3600) / 60);
//      }
	
      // Send the RPL_TIME reply
      sendNumeric(LibIRC2::Numerics::RPL_TIME,
		  config().getOptionsServerName(),
		  text.str());
      
      // Also send the RPL_TIMEONSERVERIS
      sendTimeOnServer();
      
      // We're done..
      return;
   }
   
   // Look up the server?
}


/* handleVERSION
 * Original 24/08/2001 simonb
 * 03/04/2003 simonb - Imported from old code
 */
IRC2USER_COMMAND_HANDLER(Protocol::handleVERSION)
{
   // If there are no parameters, the user was wants us to reply..
   if (parameters.empty()) {
      // Send the RPL_VERSION reply
      sendNumeric(LibIRC2::Numerics::RPL_VERSION,
		  config().getOptionsServerName(),
		  Version::version,
		  Version::versionChars);
      
      // Also send the RPL_ISUPPORT stuff
      sendISUPPORT();
      
      // We're done..
      return;
   }
   
   // Look up the server?
}
