/* utils.cc
 * General utilities
 */

#include "autoconf.h"

#include <ctype.h>
#include <stdlib.h>

#include "utils.h"


/* baseXStr - Convert a number to another base (output a string) up to base 84
 * Original 17/01/01 simonb
 * Notes: The charset is NOT mime/base64 compatible! Do not be fooled!!
 */
#define MAXBASE 84
String Utils::baseXStr(unsigned long n, unsigned short base)
{
   char const baseChrs[MAXBASE + 1] =
     "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" // 62
     "!?$#%&*+/-~[(<])>|';=";
   
   long digit;
   String tempStr = "";
   
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
bool Utils::toBool(String const &word, bool defaultBool)
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
StringMask Utils::fixToIdentityMask(String const &inMask)
{
   StringMask ret(inMask);
   return ret;
   
   String nick = "", user = "", host = "", temp1 = "", temp2 = "", temp3 = "";
   
   // Run through the given mask
   for (String::size_type i = inMask.length(); i--;) {

   }
   
   // Make the fixed mask
   StringMask retval((nick.length() ? nick : "*") + '!' +
		     (user.length() ? user : "*") + '@' +
		     (host.length() ? host : "*"));

   return retval;
}
