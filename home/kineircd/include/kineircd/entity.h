/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_ENTITY_H_
# define _INCLUDE_KINEIRCD_ENTITY_H_ 1

# include <string>
# include <aisutil/time.h>

namespace Kine {
   class User;
   
   //! Abstract base class for objects - stuff that is common to all objects
   class Entity {
    protected:
      const AIS::Util::Time signonTime;		//!< Time this entity connected
      
      //! Constructor
      explicit Entity(const AIS::Util::Time& _signonTime)
	: signonTime(_signonTime)
	{};
      
    public:
      //! Destructor
      virtual ~Entity(void)
	{};

      //! Return whatever is an acceptable name for this entity
      virtual const std::string& getName(void) const = 0;
      
      //! Is this entity hidden from the given user? (default to 'no')
      virtual const bool isHiddenFrom(const User& who) const
	{ return false; };

      //! Return the time this entity initially connected to the network
      const AIS::Util::Time& getSignonTime(void) const
	{ return signonTime; };
   }; // class Entity
}; // namespace Kine

# include <kineircd/user.h>

#endif // _INCLUDE_KINEIRCD_ENTITY_H_
