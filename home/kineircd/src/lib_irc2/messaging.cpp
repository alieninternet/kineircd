/* $Id$
 * 
 * Copyright (c) 2001,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2003 KineIRCd Development Team
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

#include <aisutil/string/string.h>
#include <aisutil/string/mask.h>
#include <aisutil/string/tokens.h>
#include <kineircd/config.h>

#include "kineircd/irc2/protocol.h"
#include "libkineircd_irc2/lang.h"

using namespace Kine::LibIRC2;
using AISutil::String;
using AISutil::StringMask;
using AISutil::StringTokens;


/* doMessage
 * Original 01/09/2001 simonb
 * Note:
 *   This gets complicated, since there are many forms:
 *    nickname
 *    nickname@server
 *    nickname%hostname
 *    nickname%hostname@server
 *    nickname!username@hostname            <=- odd one out, the @ = hostname
 *    nickname!username%hostname@server
 *    nickname#network
 *    nickname@server#network
 *    nickname%hostname#network
 *    nickname%hostname@server#network
 *    nickname!username@hostname#network    <=- also odd one out..
 *    nickname!username%hostname@server#network
 * 
 *   Channels:
 *    <char>name
 * 
 *   And of course for opers:
 *    #*.tld (user host mask)
 *    $*.tld (server host mask)
 * 
 *   .. Try to stay with me :)
 * 
 *   Can someone one day do optimisation tests and see if it really is worth
 *   keeping PRIVMSG and NOTICE separated?
 */
void Protocol::doMessage(const User& user,
			 const parameters_type& parameters,
			 const bool isNotice)
{
   // Make sure the command includes the required number of parameters
   if (parameters.size() >= 2) {
      // Tokenise the recipient list
      StringTokens recipients(parameters[0]);
      
      // Our current target
      StringMask target;

      // The number of targets left
      unsigned int targets = config().getLimitsMaxTargets();
      
      // Iterate over the parameters...
      while (!recipients.hasMoreTokens()) {
	 // Next token
	 target = recipients.nextToken(',');
	 
	 // Make sure this won't be too many targets for this user
	 if ((--targets > 0) || user.isOperator()) {
	    // Make sure this target is not empty
	    if (!target.empty()) {
	       // Make sure this is not a mask
	       if (!/* ?? */false) {
		  // Okay, check if this is a channel!
		  if (/* ?? */false) {
		     // Find the channel..
		     // something.

		     // If we got here, the channel was not found
		     if (!isNotice) {
			sendNumeric(user, Numerics::ERR_NOSUCHCHANNEL,
				    target,
				    GETLANG(irc2_ERR_NOSUCHCHANNEL));
		     }
		     continue;
		  }
		  
		  /* Okay, then presume this is a nickname.. This is the bit
		   * where it gets complicated :)
		   */
		  
	       } else if (user.isOperator()) {
		  /* Okay, we were given a mask and the caller is an IRC
		   * operator. We need to make sure the mask is indeed valid
		   * if needs be..
		   * Start by making sure there is a top level domain:
		   */
		  if (true) {
		     // Make sure there is a top level domain given
		     if (true) {
			if (!isNotice) {
			   sendNumeric(user, Numerics::ERR_NOTOPLEVEL,
				       target,
				       GETLANG(irc2_ERR_NOTOPLEVEL));
			}
			continue;
		     }
		     
		     // Make sure the oper isn't cheating by specifying a top
		     if (true) {
			if (!isNotice) {
			   sendNumeric(user, Numerics::ERR_WILDTOPLEVEL,
				       target,
				       GETLANG(irc2_ERR_WILDTOPLEVEL));
			}
			continue;
		     }
		  }
		  
		  // Okay, the mask was valid - send the message
		  // something.
		  continue;
	       }
	    }
	 } else {
	    if (!isNotice) {
	       sendNumeric(user, Numerics::ERR_TOOMANYTARGETS,
			   GETLANG(irc2_ERR_TOOMANYTARGETS_TARGET_OVERFLOW,
				   String::convert(config().
						   getLimitsMaxTargets())));
	    }
	    return;
	 }
	 
	 // If we got here, the target was not found
	 if (!isNotice) {
	    sendNumeric(user, Numerics::ERR_NOSUCHNICK,
			target,
			GETLANG(irc2_ERR_NOSUCHNICK_OR_CHANNEL));
	 }
      }
      
      // The end!
      return;
   }
   
   // If this is not a NOTICE, we are allowed to complain!
   if (!isNotice) {
      // Okay, what was missing that we need to complain about?
      if (parameters.empty()) {
	 // The recipient was missing, complain about that
	 sendNumeric(user, Numerics::ERR_NORECIPIENT,
		     GETLANG(irc2_ERR_NORECIPIENT,
			     "PRIVMSG"));
      } else {
	 // Okay, the text was missing, complain about that then
	 sendNumeric(user, Numerics::ERR_NOTEXTTOSEND,
		     GETLANG(irc2_ERR_NOTEXTTOSEND));
      }
   }
}

