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

#ifndef _INCLUDE_KINEIRCD_LOCALUSER_H_
# define _INCLUDE_KINEIRCD_LOCALUSER_H_ 1

# include <kineircd/user.h>
# include <kineircd/daemon.h>
# include <kineircd/myserver.h>

namespace Kine {
   //! A local user (refinement of a user - a user connected to this server)
   class LocalUser : public User {
    protected:
      //! Constructor
      explicit LocalUser(const ClientName& _nickname,
			 const std::string& _username,
			 const std::string& _hostname,
			 const AIS::Util::Time& _signonTime =
			 daemon().getTime())
	: User(_nickname, _username, _hostname, _signonTime)
        {};
      
    public:
      //! Destructor
      virtual ~LocalUser(void)
	{};
      
      //! Return the last time this user was known to be not idle
      virtual const AIS::Util::Time& getLastAwake(void) const = 0;


      //! Return the server this user is connect to (that is, us!)
      Server& getServer(void) const
	{ return myServer(); };


      //! Send a server notice to this user
      const Error::error_type sendServerNotice(const std::string& message)
	{ return sendNotice(myServer(), message); };
   }; // class LocalUser
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_LOCALUSER_H_
