/* $Id$
 * A user (refinement of a client)
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

#ifndef _INCLUDE_KINEIRCD_USER_H_
# define _INCLUDE_KINEIRCD_USER_H_ 1

# include <kineircd/client.h>

namespace Kine {
   class User : public Client {
    private:
      Nickname nickname;			//!< User's nickname
      std::string username;			//!< User's username
      std::string hostname;			//!< User's hostname
      std::string awayMessage;			//!< Away message (empty = off)
      
    protected:
      //! Constructor
      User(const Nickname& _nickname,
	   const std::string& _username,
	   const std::string& _hostname,
	   const timeval& _signonTime)
	: Client(_signonTime),
          nickname(_nickname),
          username(_username),
          hostname(_hostname)
	{};

    public:
      //! Destructor
      virtual ~User(void)
	{};
      
      //! Return the users's nickname
      const Nickname& getNickname(void) const
	{ return nickname; };

      //! Return the users's username / identd reply
      const std::string& getUsername(void) const
	{ return username; };

      //! Return the users's hostname
      const std::string& getHostname(void) const
	{ return nickname; };

      //! Return the virtual hostname of this user
      virtual const std::string& getVirtualHostname(void) const
	{ return getHostname(); };
      
      //! Return the away message for this user. If blank, there is none set
      const std::string& getAwayMessage(void) const
	{ return awayMessage; };
      
      //! Check if this user is away
      const bool isAway(void) const
	{ return (!getAwayMessage().empty()); };
   }; // class User
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_USER_H_
