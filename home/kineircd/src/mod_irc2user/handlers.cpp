/* $Id$
 * The IRC-2 user handlers
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

#include <aisutil/string/mask.h>

#include "mod_irc2user/protocol.h"
#include "mod_irc2user/language.h"
#include "mod_irc2user/commands.h"

using namespace Kine::mod_irc2user;


/* handleHELP
 * Original 13/08/2001 simonb
 * 02/04/2001 simonb - Imported from old code
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
	it++)
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
