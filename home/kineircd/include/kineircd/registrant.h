/* $Id$
 * Registration data structure, passed to protocol modules after registration
 * 
 * Copyright (c) 2001,2002 Simon Butcher <pickle@alien.net.au>
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

#ifndef _INCLUDE_KINEIRCD_REGISTRANT_H_
# define _INCLUDE_KINEIRCD_REGISTRANT_H_ 1

# include <queue>
# include <vector>
# include <string>
# include <aisutil/string/string.h>

namespace Kine {
   struct Registrant {
      // The password given during registration ('PASS' command)
      AISutil::String password;
      
      // Any additional data given on the PASS command after the password
      AISutil::String passwordKludge;
      
      /* The nickname, given via NICK or SERVICE. This is the network name
       * if given by IIRCN for an IIRC connection.
       */
      AISutil::String nickname;
      
      // The username, given via NICK
      AISutil::String username;
      
      // The hostname, given by USER, SERVER and IIRCN
      AISutil::String hostname;
      
      // The 'real name' or description, via USER, SERVER, SERVICE, and IIRCN
      AISutil::String realname;
      
      // The distribution field, given via SERVICE
      AISutil::String distribution;
      
      // The modes field, given by USER
      AISutil::String modes;
      
      // The protocol field, given by SERVER (or 'PROTOCOL', if implemented)
      AISutil::String protocol;
      
      // A vector listing (in order) any data sent via CAPAB
      typedef std::vector <std::string> capabilities_type;
      capabilities_type capabilities;
      
      // If the SERVER command is ircu style, these relate to the given TS info
      long startStamp;
      long linkStamp;

      // Constructor
      Registrant(void)
	: startStamp(-1),
          linkStamp(-1)
	{};
   };
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_REGISTRANT_H_
   
