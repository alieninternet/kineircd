/* $Id$
 * Parser for old style ircd.conf files - Data put into the 'Config' class
 *
 * Copyright (c) 2002 KineIRCd Development Team
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

#include <fstream>
#include <utility>
#include <stack>

#include "kineircd/config.h"
#include "kineircd/debug.h"

// Are we compiled properly?
#ifndef KINE_OLD_CONFIG_SUBPARSER
# warning "Compiling the old-style configuration sub-parser without support"
#endif


// Warn the user. This file needs a developer :(
#warning "This is majorly incomplete and will not serve any purpose. Sorry."

using namespace Kine;


/* varHandleOldConfig - Read/parse an old-style configuration file
 * Original 19/04/2002 simonb
 * HEY YOU MR. DEVELOPER! Want to work on this? I don't have any time left
 * so it's dead :( You will need to uncomment the --enable-old-config-parser
 * stuff in the configure.in file and generate a new autoconf, run it with
 * the option to be able to use this. But wait - don't commit the autoconf
 * stuff until the old config parser actually works, or does something useful.
 * At the moment it will read the file, you just need to do the definitions
 * and set the Config class appropriately. You will need to download a more
 * traditional IRC daemon though for reference. I took the formatting rules
 * from both KineIRCd but primarily the ircd found in 
 * ftp://coombs.anu.edu.au/pub/irc
 * Cheers, and good luck if you do take this on!
 */
CONFIG_VARIABLE_HANDLER(Config::varHandleOldConfig)
{
   // Check if the first value is empty
   if (values.front().empty()) {
      // Complain
      errString = "No old-style configuration file specified";
      return false;
   }
   
   // Ok then - open the file
   std::ifstream file(values.front().c_str(), std::ios::in);

   // Check that we opened the file
   if (!file) {
      errString = "Could not open the file '" + values.front() + '\'';
      return false;
   }

#ifdef KINE_DEBUG
   debug("Reading OLD-STYLE configuration data from '" + values.front() + 
	 "'...");
#endif
   
   /* Since the original ircd configuration files were line by line, they are
    * somewhat easier to parse than our primary parser (obviously). They also
    * have the unfortunate quirk that they get read BACKWARDS! In order for
    * the old configuration files to work the same way, we need to read the
    * file into memory and then parse it backwards.. For this purpose, we
    * will use a stack since it conveniently allows us to do exactly what we
    * want without anything we won't need (iterators etc).. Note that the pair
    * is used to associate line numbers with the lines conveniently for any
    * errors which may appear.. *sigh* it's horrible being so friendly :(
    * The typedef here is used to make it look neater down below where we
    * push a line and a line number to the stack.
    */
   typedef pair <unsigned long, String> linePair_type;
   stack <linePair_type> configData;
#ifdef KINE_DEBUG_ASSERT
   assert(configData.empty());
#endif
   
   // Other stuff we need
   unsigned long lineNum = 0;
   String line;
   
   // Read the file into the stack
   /* Read the entire file into our configuration data buffer, cropping away
    * redundant data such as white-space, new lines, comments etc.
    */
   for (;;) {
      // Grab a line from the file
      std::getline(file, line);

      // Make sure the file is still happy...
      if (!file.good() || file.eof()) {
	 break;
      }

      // Increase the line counter
      lineNum++;

      // Trim away leading/trailing whitespace
      line = line.trim();

      // Skip an empty line, or a line beginning with a #
      if (line.empty() || (line[0] == '#')) {
	 continue;
      }
      
      // Oh alright, add this to the stack
      configData.push(linePair_type(lineNum, line));
   }
   
   // Close the file..
   file.close();
   
   // If the stack is empty, the file was pretty pointless
   if (configData.empty()) {
      errString = 
	"No configuration data found in file '" + values.front() + '\'';
      return false;
   }
   
   // Run through the stack to parse this file...
   while (!configData.empty()) {
      // Work out what to do with this..
      switch (configData.top().second[0]) {
	 /* A-line -- Sets up administration information
	  * 
	  * Format:
	  * A:<name>:<email>:<location>::
	  */
       case 'A':

	 /* B-line -- Define alternative servers which users can be redirected
	  * to in the event that this server is 'full' (or near to it)
	  * 
	  * Format:
	  * B:<class or host mask>::<server name>:<port>:
	  */
       case 'B':
	 
	 /* C-line/c-line -- Network connect-to lines. These define where to
	  * be able to connect to in order to join an IRC network. In the
	  * traditional sense, 'C' means a normal connection, while 'c' means 
	  * compressed. Unfortunately protocols being different they both
	  * mean the same thing here (sorry!) and that being NORMAL. If you
	  * need to force a compressed link, use the main configuration file
	  * format and not this out-dated one.
	  * 
	  * Format:
	  * [C/c]:<server address>:<password>:<server name>:<port>:<class>
	  */
       case 'C':
       case 'c':

	 /* D-line -- Auto-connection advanced controls.. needs investigating?
	  * 
	  * Format:
	  * D:<denied server mask>:<denied class>:<server mask>:<server class>
	  */
       case 'D':
	 
	 /* H-line -- Hub line. Like leaf servers - obsolete?
	  * 
	  * Format:
	  * H:<permitted mask>::<server name>::
	  */
       case 'H':
	 
	 /* I-line/i-line -- Client authorisation details which assign clients
	  * to classes and determine whether they should be allowed on or
	  * not, with or without passwords etc. They work the same way, except
	  * 'I' means full access while 'i' means restricted access with 
	  * usermode '+r' set.
	  * 
	  * Format:
	  * [I/i]:<host addr>:<password>:<host name>:<port>:<class #>
	  */
       case 'I':
       case 'i':

	 /* K-line/k-line -- The (infamous) user@host mask banning mechanism. 
	  * It only applies to this server (they're local) unlike G-lines which
	  * are global. 'K' means outright, whatever information it has where
	  * as 'k' only uses the identd reply information (and is often more
	  * effective.. or something). Putting an '=' infront of the 
	  * hostname/IP field on a k-line will ban anyone matching that mask
	  * who does not return an identd reply. This implementation is a
	  * little warped! Someone work out the FULL method for this?
	  *
	  * Format:
	  * [K/k]:<hostname or IP>:<time interval(s) or comment>:<user>:<port>:
	  */
       case 'K':
       case 'k':
	 
	 /* L-line -- Define leaf servers. Obsolete? Should we even bother?
	  *
	  * Format:
	  * L:<mask to disallow>::<server name>:<depth>
	  */
       case 'L':
	 
	 /* M-line -- Sets up server name, description and the ip/port the
	  * server listens for UDP pings on (used for autoconnections)
	  * 
	  * Format:
	  * M:<hostname>:<ip>:<description>:<port>
	  */
       case 'M':

	 /* N-line -- Network lines, or which servers to allow to connect to
	  * us. See C-line or c-line for more details, since that's what they
	  * would be using on the other end.
	  * 
	  * Format:
	  * N:<server address>:<password>:<server name>:<domain mask>:<class>
	  */
       case 'N':
	 
	 /* O-line/o-line -- IRC Operator access lines. They define where
	  * an operator can come from, their password, their name, and a
	  * class to switch them to after they authenticate themselves.
	  * The 'O' lines are for global operators and the 'o' lines are for
	  * local operators.
	  * 
	  * Format:
	  * [O/o]:<host name/mask>:<password>:<nickname>:<port>:<class>
	  */
       case 'O':
       case 'o':
	 
	 /* P-line -- Ports/IP addresses to listen on. These can either be
	  * a file/directory to listen as UNIX sockets or an IPv4 address.
	  * Unfortunately limits in the configuration file format prohibit
	  * IPX or IPv6 addresses.
	  * 
	  * Format:
	  * P:<IP address>:<*>:<IP address netmask>:<port>:
	  * P:<file or directory>:<*>:<*>:<port>:
	  */
       case 'P':

	 /* Q-line -- Quarantine servers (and users + channels in KineIRCd)..
	  * Investigate the first field '*'??
	  * 
	  * Developer note: In KineIRCd this would have to "work out" what
	  * type of object it is (server/user/channel). Unfortunately this
	  * is just how it works :( :( :(
	  *
	  * Format:
	  * Q:*:<reason>:<server, user or channel name mask>
	  */
       case 'Q':
	 
	 /* R-line -- Restrict users. Works in a much more complicated way
	  * than K-lines. Worth the bother? Someone investigate? It uses
	  * external programs - eww
	  * 
	  * Format:
	  * R:<host mask>:<program>:<username>
	  */
       case 'R':
	 
	 /* S-line -- Allow services to connect to us (directly). 
	  * Investigate - this doesn't define the scope etc properly?
	  * 
	  * Format:
	  * S:<host mask>:<password>:<service name>:<service type>:<class>
	  */
       case 'S':

	 /* U-line -- Define an "arbiter" server. That is, a server which has
	  * special control on the network. What are the other fields for?
	  *
	  * In a history sense the really old irc clients used this to select
	  * the default server to connect to!
	  * 
	  * Format:
	  * U:<server name>:<unknown??>:<unknown??>
	  */
       case 'U':
	 
	 /* V-line -- Deny servers connection to you depending on parameters
	  * 
	  * Format:
	  * V:<version mask>:<ugly flags>:<server mask>::
	  */
       case 'V':
	 
	 /* X-line -- Define text for a helper type number. Obsoleted by
	  * language files.
	  * 
	  * Format:
	  * X:<code>:<text>
	  */
       case 'X':
	 
	 /* Y-line -- Connection classes. These define certain parameters
	  * for a client/service/server connection. Note that the number will
	  * be converted into a string as connection classes in kine have 
	  * proper strings (for names instead of obscure numbers). Times are 
	  * always in seconds.
	  * 
	  * Format:
	  * Y:<class #>:<ping freq>:<connect freq>:<max links>:<send-q>::
	  */
       case 'Y':
	 
       default:
	 // complain or something about an unsupported line thing..
#ifdef KINE_DEBUG
	 debug("eek. oldconfig.cpp incomplete! Should not have been compiled");
#endif
      }
      
      // Pop the top entry off the stack
      configData.pop();
   }

   // Presume everything is ok and nothing major happened
   return true;
}
