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

#include "kineircd/user.h"
#include "libkineircd/debug.h"

using namespace Kine;


/* setAway - Mark the user as being away
 * Original 13/08/2001 simonb
 */
const Error::error_type User::setAway(const std::string& reason)
{
   // Copy the message across (check it??)
   awayMessage = reason;

   // broadcast it.

   // Tell ourself about this, if we care
   doEventAwayToggle();

   return Error::NO_ERROR;
}


/* setHere - Mark the user as returning from being away
 * Original 13/08/2001 simonb
 */
const Error::error_type User::setHere(void)
{
   // Clear the away message
   awayMessage.clear();
   
   // broadcast it.

   // Tell ourself about this, if we care
   doEventAwayToggle();

   return Error::NO_ERROR;
}


/* setLanguageList - Change the language list over to the one provided
 * Original 16/04/2003 simonb
 */
const Error::error_type
  User::setLanguageList(const Languages::languageDataList_type& languages,
			const bool secret)
{
   // Okay, cheap.. We should verify the list here, really..
   languageList = languages;
   
   // broadcast it.

   // Tell ourself, if it's not a secret
   if (!secret) {
      doEventLanguageChange();
   }
   
   return Error::NO_ERROR;
}
