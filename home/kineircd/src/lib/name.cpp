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

#include <algorithm>
#include <cctype>

#include "kineircd/channelname.h"
#include "kineircd/clientname.h"
#include "kineircd/config.h"
#include "libkineircd/constants.h"

using namespace Kine;
using AISutil::String;


/* irctolower - Tiny procedure to assist in transforming IRC aliases
 * Original 12/08/2001 pickle
 */
inline static char irctolower(char c)
{
   switch (c) {
    case '[':
      return '{';
      
    case ']':
      return '}';
      
    case '\\':
      return '|';
      
    case '~':
      return '^';
   }
   
   return std::tolower(c);
}


/* isSpecialChar - Return true if the given character is 'special'
 * Original 12/08/2001 pickle
 */
inline static const bool isSpecialChar(const char chr)
{
   /* The given char is okay if it's between the range of 0x5B-0x60 ('[', '\',
    * ']', '^', '_', '`'), or within the range of 0x7B-0x7D ('{', '|', '}').
    * These two weird looking ranges are acceptable due to the scandinavian
    * origin of IRC, back a long long time ago in a universe far, far away.
    */
   return (((chr >= 0x5B) && (chr <= 0x60)) ||
	   ((chr >= 0x7B) && (chr <= 0x7D)));
}


/* IRCtoLower - Convert to lowercase while considering irc chars (eg {} and [])
 * Original 12/08/2001 pickle
 * 26/02/2002 pickle - Converted for use with std::string / AISutil::String
 */
const AISutil::String Name::IRCtoLower(void) const
{
   String result(length(), 0);
   (void)std::transform(begin(), end(), result.begin(), irctolower);
   return result;
}


/* checkValidity - Check to see if the name is valid as per nickname rules
 * Original 12/08/2001 pickle
 */
const Error::error_type ClientName::checkValidity(void) const
{
   // Check the nickname's length first
   if (length() > config().getLimitsUsersMaxNickNameLength()) {
      return Error::NAME_TOO_LONG;
   }
   
   // Check if the first character is a digit, or digit related (bad)
   if (std::isdigit((*this)[0]) ||
       ((*this)[0] == '-')) {
      return Error::NICKNAME_BEGINS_WITH_DIGIT;
   }
   
   // Run over the name and check each char..
   for (size_type i = 0; i < length(); ++i) {
      /* If the char is greater than 7-bits, it's okay (to allow for UTF-8)..
       * If the char is alphanumeric, it's okay. 
       */
      if (((*this)[i] & 0x80) ||
	  isalnum((*this)[i]) ||
	  isSpecialChar((*this)[i]) ||
	  ((*this)[i] == '-')) {
	 continue;
      }
      
      // If we got here, it's a bad char. Vewwy vewwy BAD char!!
      return Error::NAME_HAS_BAD_CHARS;
   }
   
   // Must be okay :)
   return Error::NO_ERROR;
}


/* checkValidity - Check to see if the name is valid as per channel name rules
 * Original 18/08/2001 pickle
 */
const Error::error_type ChannelName::checkValidity(void) const
{
   // Check the channel name's length
   if (length() > config().getLimitsChannelsMaxNameLength()) {
      return Error::NAME_TOO_LONG;
   }
   
   // Okay, check the first char is a prefix that we know of
   const char* prefixes = Constants::standardChannelTypePrefixes;
   while (*prefixes != '\0') {
      if ((*this)[0] == *prefixes) {
	 break;
      }
   }

   // If we reached the end of the prefix char array, we did not find it..
   if (*prefixes == '\0') {
      return Error::CHANNEL_NAME_NONSTANDARD_PREFIX;
   }
   
   /* Well okay, look for invalid chars :) This is what RFC2812 says, however
    * we may also disallow masks depending on what the configuration says
    */
   if (config().getLimitsChannelsAllowMaskLikeNames()) {
      size_type where;
      if ((where = find_first_of("\0\a\r\n ,:*?", 1)) != (size_type)-1) {
	 if (((*this)[where] == '*') ||
	     ((*this)[where] == '?')) {
	    return Error::NAME_IS_A_MASK;
	 }
	 
	 return Error::NAME_HAS_BAD_CHARS;
      }
   } else {
      if (find_first_of("\0\a\r\n ,:", 1) != (size_type)-1) {
	 return Error::NAME_HAS_BAD_CHARS;
      }
   }
   
   // Looks good, chief
   return Error::NO_ERROR;
}
