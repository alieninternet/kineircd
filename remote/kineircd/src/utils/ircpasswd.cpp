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
#ifdef HAVE_TERMIOS_H
# include <termios.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
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

   // Check if we are being called in script form or not
   if (argc > 2) {
      // Grab the two variables
      nickname = argv[1];
      password = argv[2];
      
      // Check.
      if (!nickname.length() || !password.length()) {
	 return 1;
      }
      
      // Simply convert it and pipe it to std output
      std::cout <<
	AISutil::SHA1::digestToStr(Password::makePassword(nickname, password),
				   Config::operPasswordStrBase,
				   Config::operPasswordStrBaseLongPad) <<
	std::endl;
      
      // Done.
      return 0;
   } 
   
   // Welcome the user, since this is 'interactive' mode
   std::cout << Version::versionFull << "\nPassword Generation Utility\n" << 
     std::endl;

   // Read in the nickname
   std::cout << "Nickname: " << std::flush;
   std::cin >> nickname;

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
   
   // Output what to add to the configuration file
   std::cout << "\n\nConfiguration data for OPERATORS class:\n\t" << 
     nickname << " {\n\t   password = \"" << 
     AISutil::SHA1::digestToStr(Password::makePassword(nickname, password),
				Config::operPasswordStrBase,
				Config::operPasswordStrBaseLongPad) <<
     "\";\n\t}\n\nDon't forget to add the 'name' and 'type' variables, and "
     "the 'hosts' sub-class\nto complete the configuration of this "
     "operator." << std::endl;

   return 0;
}

