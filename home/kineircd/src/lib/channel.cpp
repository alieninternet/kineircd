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
#include "kineircd/kineircdconf.h"

#include "kineircd/entity.h"
#include "kineircd/channel.h"
#include "kineircd/config.h"
#include "lib/constants.h"
#include "lib/debug.h"

using namespace Kine;


/* checkValidity - Check to see if the name is valid as per channel name rules
 * Original 18/08/2001 pickle
 */
const Error::error_type Channel::Name::checkValidity(void) const
{
   // Check the channel name's length
   if (length() > config().getLimitsChannelsMaxNameLength()) {
      return Error::NAME_TOO_LONG;
   }
   
   // Okay, check the first char is a prefix that we know of
   const wchar_t* prefixes = Constants::standardChannelTypePrefixes;
   while (*prefixes != L'\0') {
      if ((*this)[0] == *prefixes) {
	 break;
      }
   }

   // If we reached the end of the prefix char array, we did not find it..
   if (*prefixes == L'\0') {
      return Error::CHANNEL_NAME_NONSTANDARD_PREFIX;
   }
   
   /* Well okay, look for invalid chars :) This is what RFC2812 says, however
    * we may also disallow masks depending on what the configuration says
    */
   if (config().getLimitsChannelsAllowMaskLikeNames()) {
      size_type where;
      if ((where = find_first_of(L"\0\007\012\015 ,:*?", 1)) != npos) {
	 if (((*this)[where] == L'*') ||
	     ((*this)[where] == L'?')) {
	    return Error::NAME_IS_A_MASK;
	 }
	 
	 return Error::NAME_HAS_BAD_CHARS;
      }
   } else {
      if (find_first_of(L"\0\007\012\015 ,:", 1) != npos) {
	 return Error::NAME_HAS_BAD_CHARS;
      }
   }
   
   // Looks good, chief
   return Error::NO_ERROR;
}
