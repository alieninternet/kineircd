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

#include <cctype>

#include "kineircd/entity.h"
#include "kineircd/client.h"
#include "kineircd/config.h"
#include "lib/debug.h"

using namespace Kine;


/* isSpecialChar - Return true if the given character is 'special'
 * Original 12/08/2001 pickle
 */
inline static const bool isSpecialChar(const wchar_t chr)
{
   /* The given char is okay if it's between the range of 0x5B-0x60 ('[', '\',
    * ']', '^', '_', '`'), or within the range of 0x7B-0x7D ('{', '|', '}').
    * These two weird looking ranges are acceptable due to the scandinavian
    * origin of IRC, back a long long time ago in a universe far, far away.
    */
   return (((chr >= 0x005B) && (chr <= 0x0060)) ||
	   ((chr >= 0x007B) && (chr <= 0x007D)));
}


/* checkValidity - Check to see if the name is valid as per nickname rules
 * Original 12/08/2001 pickle
 */
const Error::error_type Client::Name::checkValidity(void) const
{
   // Check the nickname's length first
   if (length() > config().getLimitsUsersMaxNickNameLength()) {
      return Error::NAME_TOO_LONG;
   }
   
   // Check if the first character is a digit, or digit related (bad)
   if (std::isdigit((*this)[0]) ||
       ((*this)[0] == L'-')) {
      return Error::NICKNAME_BEGINS_WITH_DIGIT;
   }
   
   // Run over the name and check each char..
   for (size_type i = 0; i < length(); ++i) {
      /* If the char is greater than 7-bits, it's okay (to allow for UTF-8)..
       * If the char is alphanumeric, it's okay. 
       */
      if (((*this)[i] & 0x0080) ||
	  std::isalnum((*this)[i]) ||
	  isSpecialChar((*this)[i]) ||
	  ((*this)[i] == L'-')) {
	 continue;
      }
      
      // If we got here, it's a bad char. Vewwy vewwy BAD char!!
      return Error::NAME_HAS_BAD_CHARS;
   }
   
   // Must be okay :)
   return Error::NO_ERROR;
}
