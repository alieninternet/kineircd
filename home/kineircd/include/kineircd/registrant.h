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

#ifndef _INCLUDE_KINEIRCD_REGISTRANT_H_
# define _INCLUDE_KINEIRCD_REGISTRANT_H_ 1

# include <queue>
# include <vector>
# include <string>
# include <aisutil/string/string.h>

namespace Kine {
   //! Registration data structure, passed to protocol modules after registration
   struct Registrant {
      // The password given during registration ('PASS' command)
      std::wstring password;
      
      // Any additional data given on the PASS command after the password
      std::wstring passwordKludge;
      
      /* The nickname, given via NICK or SERVICE. This is the network name
       * if given by IIRCN for an IIRC connection.
       */
      std::wstring name;
      
      // The username, given via NICK
      std::wstring username;
      
      // The hostname, given by USER, SERVER and IIRCN
      std::wstring hostname;
      
      // The 'real name' or description, via USER, SERVER, SERVICE, and IIRCN
      std::wstring description;
      
      // The distribution field, given via SERVICE
      std::wstring distribution;
      
      // The modes field, given by USER
      std::string modes;
      
      // The protocol field, given by SERVER (or 'PROTOCOL', if implemented)
      AIS::Util::String protocol;
      
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
   
