/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
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

#include <algorithm>
#include <aisutil/string.h>

#include "kineircd/user.h"
#include "kineircd/registry.h"
#include "kineircd/myserver.h"
#include "lib/debug.h"

using namespace Kine;
using AISutil::String;


// Static stuff..
const std::string::size_type User::maxStaffStatusLength = 32;


/* changeNickname - Change this user's nickname
 * Original 16/05/2003 simonb
 */
const Error::error_type User::changeNickname(const Denizen& changer,
					     const ClientName& newNickname)
{
   // Check the nickname is valid
   Error::error_type validityError;
   if ((validityError = newNickname.checkValidity()) != Error::NO_ERROR) {
      return validityError;
   }
   
   // Make sure the nickname is not already in use
   const Client* const foundClient = registry().findClient(newNickname);
   if ((foundClient != 0) && (foundClient != this)) {
      return Error::NAME_IS_IN_USE;
   }
   
   // Tell ourself about the change
   doEventNicknameChange(changer, *this, newNickname);

   // Tell the registry
   const Error::error_type registryError =
     registry().changeUserNickname(*this, newNickname);
     
   // broadcast it.
    
   // Change the nickname.. (last thing to do)
   nickname = newNickname;
   
   // Return whatever the registry said happened
   return registryError;
}


/* setAway - Mark the user as being away
 * Original 13/08/2001 simonb
 */
const Error::error_type User::setAway(const Denizen& changer,
				      const std::string& reason)
{
   // Copy the message across (check it??)
   awayMessage = static_cast<const String&>(reason).trim();

   // broadcast it.

   // Tell ourself about this, if we care
   doEventAwayToggle(changer);

   return Error::NO_ERROR;
}


/* setHere - Mark the user as returning from being away
 * Original 13/08/2001 simonb
 */
const Error::error_type User::setHere(const Denizen& changer)
{
   // Clear the away message
   awayMessage.clear();
   
   // broadcast it.

   // Tell ourself about this, if we care
   doEventAwayToggle(changer);

   return Error::NO_ERROR;
}


/* setLanguageList - Change the language list over to the one provided
 * Original 16/04/2003 simonb
 */
const Error::error_type
  User::setLanguageList(const Denizen& changer,
			const Languages::languageDataList_type& languages,
			const bool secret)
{
   // Okay, cheap.. We should verify the list here, really..
   languageList = languages;
   
   // broadcast it.

   // Tell ourself, if it's not a secret
   if (!secret) {
      doEventLanguageChange(changer);
   }
   
   return Error::NO_ERROR;
}


/* changeStaffStatus - Change the staff status
 * Original 29/04/2003 simonb
 */
const Error::error_type User::changeStaffStatus(const Denizen& changer,
						const std::string& status)
{
   // Naughty chars we consider a delimeter
   static const char* const naughtyChars =
     "\0!\"#$%&'()*+,-./\\:;<=>?@[\\]^`{|}~";
   
   // Make sure the given status is not empty..
   if (!status.empty()) {
      /* Set the status, making sure the status is entirely upper-case, is
       * only one word, and is smaller than the maximum length allowed.
       */
      staffStatus =
	static_cast<const String&>
	(status.substr(0,
		       std::min(maxStaffStatusLength,
				status.find_first_of(naughtyChars)))).toUpper();
      
      // broadcast it.

      // Tell ourself..
      doEventStaffStatusChange(changer);
      
      // No worries
      return Error::NO_ERROR;
   }
   
   // Okay, the status was empty - bounce into 'setStaffOff' then..
   return setStaffOff(changer);
}


/* setStaffOff - Remove the staff status
 * Original 29/04/2003 simonb
 */
const Error::error_type User::setStaffOff(const Denizen& changer)
{
   // Clear the status..
   staffStatus.clear();
   
   // broadcast it..
   
   // Tell ourself....
   doEventStaffStatusChange(changer);
   
   // Happy chappy..
   return Error::NO_ERROR;
}


/* sendMessage - Handle the reception of a message sent to us
 * Original 12/05/2003 simonb
 */
const Error::error_type
  User::sendMessage(Sender& from, const std::string& message,
		    const Receiver::Directivity directivity)
{
   // Lazy for now, just accept the message
   doEventReceivePrivateMessage(from, message);
   
   return Error::NO_ERROR;
}


/* sendNotice - Handle the reception of a notice sent to us
 * Original 12/05/2003 simonb
 */
const Error::error_type
  User::sendNotice(Sender& from, const std::string& message,
		   const Receiver::Directivity directivity)
{
   // Lazy for now, just accept the notice
   doEventReceivePrivateNotice(from, message);

   return Error::NO_ERROR;
}


/* getServer - Return the server local users are connected to (ie. 'my server')
 * Original 20/06/2003 simonb
 */
Server& LocalUser::getServer(void) 
{
   return myServer();
}
