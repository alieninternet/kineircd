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
#include <kineircd/irc2/utility.h>

#include "mod_irc2user/protocol.h"
#include "mod_irc2user/lang.h"

using namespace Kine::mod_irc2user;
using AIS::Util::String;
using AIS::Util::StringMask;
using AIS::Util::StringTokens;


/* doMessage
 * Original 01/09/2001 simonb
 * Note: Can someone one day do optimisation tests and see if it really is
 *       worth separating PRIVMSG and NOTICE?
 */
void Protocol::doMessage(const parameters_type& parameters,
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
      while (recipients.hasMoreTokens()) {
	 // Next token
	 target = recipients.nextToken(',');
	 
	 // Make sure this won't be too many targets for this user
	 if ((--targets > 0) || user.isOperator()) {
	    // Make sure this target is not empty
	    if (!target.empty()) {
	       // Make sure this is not a mask
	       if (!/* ?? */false) {
		  // The directivity (how acutely the target was specified)
		  Receiver::Directivity directivity;
	    
		  // Find something of this name
		  Receiver* const receiver =
		    LibIRC2::Utility::findMessageTarget(target, directivity, 
							true
							/* config ^^^ ? */);
		  
		  // Did we find something?
		  if (receiver != 0) {
		     // Check if we can send to this target
		     
		     // Okay, send the message finally
		     if (!isNotice) {
			// Send the message, remembering the error
			const Error::error_type error =
			  receiver->sendMessage(user, parameters[1],
						directivity);

			// Was there no error (most likely)?
			if (error == Error::NO_ERROR) {
			   // If this was a user, check if they were away
			   const User* const foundUser =
			     dynamic_cast<const User* const>(receiver);
			   if ((foundUser != 0) && (foundUser->isAway())) {
			      sendNumeric(LibIRC2::Numerics::RPL_AWAY,
					  foundUser->getName(),
					  foundUser->getAwayMessage());
			   }
			   
			   // Skip to the next target..
			   continue;
			}

			// Was this target a channel?
			const bool channelTarget =
			  (dynamic_cast<Channel* const>(receiver) != 0);
			
			/* Check the error nicely.. The cast here is used to
			 * keep the compiler quiet, otherwise it'll moan about
			 * not including *EVERY* error (which we obviously
			 * cannot and do not want to do!)
			 */
			switch ((int)error) {
			 // Sending messages is unsupported! (ouch)
			 case Error::UNSUPPORTED_BY_ENTITY:
			   if (channelTarget) {
			      sendNumeric
				(LibIRC2::Numerics::ERR_CANNOTSENDTOCHAN,
				 target,
				 GETLANG(irc2_ERR_CANNOTSENDTOCHAN_UNRECEPTIVE));
			   } else {
			      sendNumeric
				(LibIRC2::Numerics::ERR_CANNOTSENDTONICK,
				 target,
				 GETLANG(irc2_ERR_CANNOTSENDTONICK_UNRECEPTIVE));
			   }
			   continue;
			   
			 // This was an unexpected error.. not friendly! :(
			 default:
			   if (channelTarget) {
			      sendNumeric
				(LibIRC2::Numerics::ERR_CANNOTSENDTOCHAN,
				 target,
				 GETLANG(irc2_ERR_CANNOTSENDTOCHAN_UNEXPECTED));
			   } else {
			      sendNumeric
				(LibIRC2::Numerics::ERR_CANNOTSENDTONICK,
				 target,
				 GETLANG(irc2_ERR_CANNOTSENDTONICK_UNEXPECTED));
			   }
			}
		     } else {
			// Send a notice, and we do not care if it worked
			(void)receiver->sendNotice(user, parameters[1],
						   directivity);
		     }
		     continue;
		  }
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
			   sendNumeric(LibIRC2::Numerics::ERR_NOTOPLEVEL,
				       target,
				       GETLANG(irc2_ERR_NOTOPLEVEL));
			}
			continue;
		     }
		     
		     // Make sure the oper isn't cheating by specifying a top
		     if (true) {
			if (!isNotice) {
			   sendNumeric(LibIRC2::Numerics::ERR_WILDTOPLEVEL,
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
	       sendNumeric(LibIRC2::Numerics::ERR_TOOMANYTARGETS,
			   GETLANG(irc2_ERR_TOOMANYTARGETS,
				   String::convert(config().
						   getLimitsMaxTargets())));
	    }
	    return;
	 }
	 
	 // If we got here, the target was not found
	 if (!isNotice) {
	    sendNumeric(LibIRC2::Numerics::ERR_NOSUCHNICK,
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
	 sendNumeric(LibIRC2::Numerics::ERR_NORECIPIENT,
		     GETLANG(irc2_ERR_NORECIPIENT,
			     "PRIVMSG"));
      } else {
	 // Okay, the text was missing, complain about that then
	 sendNumeric(LibIRC2::Numerics::ERR_NOTEXTTOSEND,
		     GETLANG(irc2_ERR_NOTEXTTOSEND));
      }
   }
}

