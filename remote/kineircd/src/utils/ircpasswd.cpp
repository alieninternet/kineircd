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

#include <iostream>
#include <string>
#include <cstdlib>
#include <locale>
#ifdef HAVE_TERMIOS_H
# include <termios.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_ICONV_H
# include <iconv.h>
#endif
#ifdef HAVE_LANGINFO_H
# include <langinfo.h>
#endif
#ifdef HAVE_WCHAR_H
# include <wchar.h>
#endif
#include <kineircd/password.h>
#include <kineircd/config.h>
#include <kineircd/version.h>

using namespace Kine;


/* main
 * Original 08/09/2001 simonb
 */
int main(int argc, char **argv)
{
   std::string nickname, password;

   // Try to grab the current locale..
   (void)setlocale(LC_CTYPE, "");
   
   // Check if we are being called in script form or not
   if (argc > 2) {
      // Grab the two variables
      nickname = argv[1];
      password = argv[2];
      
      // Check.
      if (!nickname.length() || !password.length()) {
	 return 1;
      }
   } else { 
      // Welcome the user, since this is 'interactive' mode
      std::cout << Version::versionFull << "\nPassword Generation Utility\n" <<
	std::endl;
      
      // Ask for the nickname
      std::cout << "Nickname: ";
      
      // Was a nickname specified?
      if (argc > 1) {
	 // Set, and output the given nickname and continue on
	 std::cout << (nickname = argv[1]) << std::endl;
      } else {
	 // Okay, read in the nickname
	 std::cout << std::flush;
	 std::cin >> nickname;
      }

#ifdef HAVE_TCGETATTR
      /* Stop the standard input echo, just in case someone is reading over
       * the users shoulder :)
       */
      struct termios stdinOrigIOS, stdinNewIOS;
      tcgetattr(0, &stdinOrigIOS);
      stdinNewIOS = stdinOrigIOS;
      stdinNewIOS.c_lflag &= ~ECHO;
      tcsetattr(0, TCSANOW, &stdinNewIOS);
#endif
   
      // Read in the password
      std::cout << "Password: " << std::flush;
      std::cin >> password;
      
#ifdef HAVE_TCGETATTR
      // Restore the standard input IOS modes (some TTYs may appreciate this)
      tcsetattr(0, TCSANOW, &stdinOrigIOS);
#endif
   }
   
   /* Since we need to convert the name/password into our internal charset,
    * we need to open an iconv() conversion descriptor.. Let's try to do that
    * using information from the locale (hopefully)
    */
   iconv_t convDesc = iconv_open(KINE_INTERNAL_CHARSET,
				 nl_langinfo(CODESET));

   wchar_t buffer[1024]; // = ~4kbytes, should be enough?

   // Make sure the descriptor opened okay, otherwise we will have problems..
   if (convDesc == (iconv_t)(-1)) {
      std::cout << "Cannot convert from your character set to "
	KINE_INTERNAL_CHARSET << std::endl;
      return 1;
   }
   
   // Convert the nickname
   char* inPtr = (char*)nickname.data();
   char* outPtr = (char*)buffer;
   size_t inBytes = nickname.length();
   size_t outBytes = sizeof(buffer) * sizeof(wchar_t);
   (void)iconv(convDesc, NULL, NULL, &outPtr, &outBytes);
   
   if (iconv(convDesc, &inPtr, &inBytes, &outPtr, &outBytes) == (size_t)(-1)) {
      std::cout << "Unable to convert nickname to " KINE_INTERNAL_CHARSET <<
	std::endl;
      return 1;
   }
   
   std::wstring nick(buffer, (((int)outPtr - (int)buffer) / sizeof(wchar_t)));
   
   // Convert the password
   inPtr = (char*)password.data();
   outPtr = (char*)buffer;
   inBytes = password.length();
   outBytes = sizeof(buffer) * sizeof(wchar_t);
   (void)iconv(convDesc, NULL, NULL, &outPtr, &outBytes);

   if (iconv(convDesc, &inPtr, &inBytes, &outPtr, &outBytes) == (size_t)(-1)) {
      std::cout << "Unable to convert password to " KINE_INTERNAL_CHARSET <<
	std::endl;
      return 1;
   }
   
   std::wstring pass(buffer, (((int)outPtr - (int)buffer) / sizeof(wchar_t)));
   
   // Generate the password
   std::string pwdhash =
     Password(nick, pass).toStr(Config::operPasswordStrBase,
				Config::operPasswordStrBaseLongPad);
   
   // Depending on how we were called, output the hashed password
   if (argc > 2) {
      std::cout << pwdhash << std::endl;
   } else {
      // Output what to add to the configuration file
      std::cout << "\n\nConfiguration data for OPERATORS class:\n\t" << 
	nickname << " {\n\t   password = \"" << pwdhash <<
	"\";\n\t}\n\nDon't forget to add the 'name' and 'type' variables, and "
	"the 'hosts' sub-class\nto complete the configuration of this "
	"operator." << std::endl;
   }
      
   return 0;
}

