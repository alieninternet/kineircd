/* utils.cc
 * General utilities
 */

#include "config.h"

#include <ctype.h>
#include <stdlib.h>

#include "utils.h"


/* isIRCspecial - Check if a char fits into the 'special' category
 * Original 20/08/01, Simon Butcher <pickle@austnet.org>
 * Note: Follows RFC2812
 */
bool isIRCspecial(char c)
{
   if (((c > 0x5A) && (c < 0x61)) ||
       ((c > 0x7A) && (c < 0x7E))) {
      return true;
   }
   
   return false;
}


/* isChannel - Blunt channel checking routine that just checks the first char
 * Original 22/08/01, Simon Butcher <pickle@austnet.org>
 */
bool isChannel(String const &channel)
{
   switch (channel[0]) {
    case '#':
    case '&':
    case '+':
    case '!':
      return true;
   }
   
   return false;
}


/* okNickname - Check if a nickname contains ok characters and is short enough
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 */
bool okNickname(String const &nick)
{
   // Check that the nickname is within limits
   if (nick.length() > MAXLEN_NICKNAME) {
      return false;
   }

   // Check the first char, it cannot be a number
   if (isdigit(nick[0])) {
      return false;
   }
   
   // Run through the nickname and look for nasty characters
   for (String::length_t i = nick.length(); i--;) {
      char c = nick[i];

      if (!(isalnum(c) || 
	    isIRCspecial(c) ||
	    (c == '-'))) {
	 return false;
      }
   }

   return true;
}


/* okChannel - Check if a channel name is ok
 * Original 20/08/01, Simon Butcher <pickle@austnet.org>
 */
bool okChannel(String const &channel)
{
   // Check that the nickname is within limits
   if (channel.length() > MAXLEN_CHANNELNAME) {
      return false;
   }

   // Check the first char of the channel
   if (!isChannel(channel)) {
      return false;
   }

   // Run through the nickname and look for nasty characters
   for (String::length_t i = channel.length(); i--;) {
#ifdef STRICT_CHANNEL_NAMES
      /* Check for nasty chars. Notice the second check is for a blank char
       * ("No-Break Space" according to the ISO), and people use it especially
       * as an alternative to ordinary space to get around protocol limitations
       * and to spoof channels, eg. "#Foo " in a channel list can look like 
       * "#Foo" in many, if not all clients.
       */
      if (channel[i] < 33) {
	 return false;
      }
#endif
      
      switch (channel[i]) {
#ifdef STRICT_CHANNEL_NAMES
       case char(0xA0): // Used by a lot of people as an alternative space
#else
       case '\0': // Nasty chars less than char 33
       case '\a':
       case '\r':
       case '\n':
       case ' ':
#endif
       case ',': // Chars used in the protocol
       case ':':
	 return false;
      }
   }

   return true;
}


/* baseXStr - Convert a number to another base (output a string) up to base 84
 * Original 17/1/01, Simon Butcher <simonb@alien.net.au>
 * Notes: The charset is NOT mime/base64 compatible! Do not be fooled!!
 */
#define MAXBASE 84
String baseXStr(unsigned long n, unsigned short base)
{
   const char baseChrs[MAXBASE + 1] =
     "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" // 62
     "!?$#%&*+/-~[(<])>|';=";
   
   long digit;
   String tempStr = "";
   
   while (n > 0) {
      digit = n % base;
      n = (n - digit) / base;
      tempStr = String(baseChrs[digit]) + tempStr;
   }
   
   return tempStr;
}


/* getRandom - Return a random integer between 0 and the number given
 * Original 16/7/01, Simon Butcher <simonb@alien.net.au>
 */
unsigned long getRandom(unsigned long max)
{
   return (unsigned long)(((max+1.0) * rand()) / RAND_MAX);
}


/* toBool - Convert a string like 'yes' 'no' 'true' 'false' etc into boolean
 * Original 21/09/01, Simon Butcher <pickle@austnet.org>
 * Note: This is very rough.
 */
bool toBool(String const &word, bool defaultBool)
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


/* makeOperPassword - Transform the nickname and password pair into SHA1 output
 * Original 08/09/01, Simon Butcher <pickle@austnet.org>
 */
String makeOperPassword(String const &nickname, String const &password)
{
   String compiled = String::printf("%s" MAGIC_OPER_PASSWD_STRING "%s",
				    (const char *)nickname.IRCtoLower(),
				    (const char *)password);
   
   return generateSHA1(compiled);
}


/* fixToIdentityMask - Transform a normal mask into an 'identity mask'
 * Original , Simon Butcher <pickle@austnet.org>
 */
StringMask fixToIdentityMask(String const &inMask)
{
   StringMask ret(inMask);
   return ret;
   
   String nick = "", user = "", host = "", temp1 = "", temp2 = "", temp3 = "";
   
   // Run through the given mask
   for (String::length_t i = inMask.length(); i--;) {

   }
   
   // Make the fixed mask
   StringMask retval(String::printf("%s!%s@%s",
				    (nick.length() ?
				     (const char *)nick :
				     "*"),
				    (user.length() ?
				     (const char *)user :
				     "*"),
				    (host.length() ?
				     (const char *)host :
				     "*")));

   return retval;
}
