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

#ifndef _INCLUDE_KINEIRCD_PASSWORD_H_
# define _INCLUDE_KINEIRCD_PASSWORD_H_ 1

# include <string>
# include <aisutil/sha1.h>
# include <kineircd/clientname.h>

namespace Kine {
   //! Password class
   class Password {
    public:
      typedef AIS::Util::SHA1::digest_type digest_type;
      
    private:
      // DO NOT CHANGE THIS, unless you want to be hurt very painfully :(
      static const char delim = '\0';		//!< Security delimeter :)

      digest_type const password;		//!< Password (hashed)
      
    public:
      // Constructor   
      Password(const digest_type& p)
	: password(p)
	{};

      // Destructor
      ~Password(void)
	{};
      
      // Return the password digest
      const digest_type& getPassword(void) const {
	 return password;
      };
      
      // Transform the nickname and password pair into SHA1 output
      static digest_type makePassword(const ClientName& nickname,
				      const std::string& password) {
	 return AIS::Util::SHA1::generate(nickname.IRCtoLower() + delim + 
					  password);
      };
   };
};
   
#endif // _INCLUDE_KINEIRCD_PASSWORD_H_

