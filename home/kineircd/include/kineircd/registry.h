/* $Id$
 * The registry, the place where all entities live
 * 
 * Copyright (c) 2001,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2003 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_REGISTRY_H_
# define _INCLUDE_KINEIRCD_REGISTRY_H_ 1

# ifdef KINE_STL_HAS_HASH
#  include <hash_map>
# endif
# include <map>

# include <kineircd/network.h>
# include <kineircd/server.h>
# include <kineircd/channel.h>
# include <kineircd/service.h>
# include <kineircd/localuser.h>
# include <kineircd/errors.h>

namespace Kine {
   class Registry {
    public:
      //! Network-wide connected users list type
# ifdef KINE_STL_HAS_HASH
      typedef std::hash_map < Name, User* const > users_type;
# else
      typedef std::map < Name, User* const > users_type;
# endif

      //! Locally connected users list type
# ifdef KINE_STL_HAS_HASH
      typedef std::hash_map < Name, LocalUser* const > localUsers_type;
# else
      typedef std::map < Name, LocalUser* const > localUsers_type;
# endif

    private:
      //! Network-wide connected users (includes users local, and network wide)
      users_type users;
      
      //! List of locally connected users
      localUsers_type localUsers;
      
      //! Our single instance
      static Registry* instance;
      
      //! Constructor
      Registry(void);
      
    public:
      //! Destructor
      ~Registry(void)
	{};
      
      //! Create the single instance of this class
      static void initInstance(void);
      
      //! Get the single instance of this class
      static Registry& getInstance(void)
	{ return *instance; };
      
      //! Add the given user
      Error::error_type add(User& entity);
      
      //! Add the given local user
      Error::error_type add(LocalUser& entity);
      
      //! Remove the given user
      Error::error_type remove(const User& entity);

      //! Remove the given local user
      Error::error_type remove(const LocalUser& entity);
      
      //! Find the given local user, by its name
      User* const findUser(const Name& name) const;
      
      //! Find the given local user, by its name
      LocalUser* const findLocalUser(const Name& name) const;
      
      //! Find the given client, by its name
      Client* const findClient(const Name& name) const
	{ return findUser(name); /* <=- temporary */ };
      
      //! Return the local user list (read-only access)
      const users_type& getUsers(void) const
	{ return users; };
      
      //! Return the local user list (read-only access)
      const localUsers_type& getLocalUsers(void) const
	{ return localUsers; };
   }; // class Registry
   

   //! Return the current instance of the Registry:: class
   inline static Registry& registry(void)
     { return Registry::getInstance(); };
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_REGISTRY_H_
