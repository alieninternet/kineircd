/* $Id$
 * General utilities
 *
 * Copyright (C) 2001,2002 Alien Internet Services
 * Copyright (c) 2001,2002 KineIRCd Development Team
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

#include "kineircd/kineircdconf.h"

#include <cctype>
#include <cstdlib>

#include "kineircd/utils.h"


/* baseXStr - Convert a number to another base (output a string) up to base 85
 * Original 17/01/2001 simonb
 * 06/04/2001 simonb - Updated the baseChrs string to produce base-85 output
 * Note: The charset is NOT mime/base64-encoding compatible!!!
 */
#define MAXBASE 85
String Utils::baseXStr(unsigned long n, const base_type base)
{
   const char baseChrs[MAXBASE + 1] =
     "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" // 62 
     "!?$#%&*+-/:<=>@:[]^{|}~";				// + 33 = 85 chars

#ifdef DEBUG_ASSERT
   // Make sure the base is within the limits..
   assert((base > 1) && (base <= MAXBASE));
#endif
   
   long digit;
   String tempStr;
   
   while (n > 0) {
      digit = n % base;
      n = (n - digit) / base;
      tempStr = baseChrs[digit] + tempStr;
   }
   
   return tempStr;
}


/* toBool - Convert a string like 'yes' 'no' 'true' 'false' etc into boolean
 * Original 21/09/01 simonb
 * Note: This is very rough.
 */
bool Utils::toBool(const String &word, const bool defaultBool)
{
   String newWord = word.toUpper();
   
   if ((newWord == "YES") ||
       (newWord == "TRUE") ||
       (newWord == "ON")) {
      return true;
   }
   
   if ((newWord == "NO") ||
       (newWord == "FALSE") ||
       (newWord == "OFF")) {
      return false;
   }
   
   return defaultBool;
}


/* fixToIdentityMask - Transform a normal mask into an 'identity mask'
 * Original  simonb
 */
StringMask Utils::fixToIdentityMask(const String &inMask)
{
   StringMask ret(inMask);
   return ret;
   
   String nick, user, host, temp1, temp2, temp3;
   
   // Run through the given mask
   for (String::size_type i = inMask.length(); i--;) {

   }
   
   // Make the fixed mask
   StringMask retval((nick.length() ? nick : "*") + '!' +
		     (user.length() ? user : "*") + '@' +
		     (host.length() ? host : "*"));

   return retval;
}
