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

namespace Kine {
   class Registry {
    private:
      //! Locally connected users list type
# ifdef KINE_STL_HAS_HASH
      typedef std::hash_map < Name, LocalUser* > localUsers_type;
# else
      typedef std::map < Name, LocalUser* > localUsers_type;
# endif
      
      //! Our single instance
      static Registry* instance;
      
      //! Constructor
      Registry(void);
      
    public:
      //! List of locally connected users (these are also in the netwide table)
      localUsers_type localUsers;
      
      //! Destructor
      ~Registry(void)
	{};
      
      //! Create the single instance of this class
      static void initInstance(void);
      
      //! Get the single instance of this class
      static Registry& getInstance(void)
	{ return *instance; };
   }; // class Registry
   

   //! Return the current instance of the Registry:: class
   inline static Registry& registry(void)
     { return Registry::getInstance(); };
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_REGISTRY_H_
