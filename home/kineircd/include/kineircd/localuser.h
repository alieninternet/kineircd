/* $Id$
 * A local user (refinement of a user - a user connected to this server)
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

# include <ctime>

# include <kineircd/user.h>

namespace Kine {
   class LocalUser : public User {
    private:
      time_t lastNickChange;			//!< Last nickname change time
      time_t lastAwake;				//!< Time this user was 'awake'
      
    public:
      //! Constructor
      LocalUser(void)
	{};
      
      //! Destructor
      virtual ~LocalUser(void)
	{};
      
      //! Return the last time this nickname was changed by the user
      const time_t& getLastNickChange(void) const
	{ return lastNickChange; };
      
      //! Return the last time this user was known to be not idle
      const time_t& getLastAwake(void) const
	{ return lastAwake; };
   };
};

#endif // _INCLUDE_KINEIRCD_LOCALUSER_H_
