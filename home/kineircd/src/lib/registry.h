/* $Id$
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

#ifndef _SRC_LIB_REGISTRY_H_
# define _SRC_LIB_REGISTRY_H_ 1

# ifdef KINE_STL_HAS_HASH
#  include <hash_map>
# else
#  include <map>
# endif

# include <kineircd/user.h>
# include <kineircd/service.h>
# include <kineircd/server.h>
# include <kineircd/network.h>
# include <kineircd/channel.h>
# include <kineircd/errors.h>


namespace Kine {
   namespace Internal {
      // The registry, the place where all entities live
      class Registry {
       private:
	 // Connected users list type
	 typedef KINE_HASH_MAP_TYPE < Client::Name, User* const >
	   users_type;
      
	 // Connected services list type
	 typedef KINE_HASH_MAP_TYPE < Client::Name, Service* const >
	   services_type;
	 
	 // Connected servers list type
	 typedef KINE_HASH_MAP_TYPE < Server::Name, Server* const >
	   servers_type;
	 
	 // Connected networks list type
	 typedef KINE_HASH_MAP_TYPE < Network::Name, Network* const >
	   networks_type;
	 
	 // Known channel list type
	 typedef KINE_HASH_MAP_TYPE < Channel::Name, Channel* const >
	   channels_type;
	 
	 // Connected users list
	 users_type users;
	 
	 // Connected services list
	 services_type services;
      
	 // Connected servers list
	 servers_type servers;
	 
	 // Connected networks list
	 networks_type networks;
	 
	 // Known channel list
	 channels_type channels;
	 
	 
	 // Our single instance
	 static Registry* instance;
	 
	 // Constructor
	 Registry(void);
	 
       public:
	 // Destructor
	 ~Registry(void)
	   {};

	 // Create the single instance of this class
	 static void initInstance(void);
	 
	 // Get the single instance of this class
	 static Registry& getInstance(void)
	   { return *instance; };

	 User* const findUser(const User::Name& name);
	 const Error::error_type addUser(User& user);
	 const Error::error_type removeUser(const User& user);
	 const Error::error_type
	   changeUserNickname(User& user, const User::Name& newNickname);

	 Service* const findService(const Service::Name& name);
	 const Error::error_type addService(Service& service);
	 const Error::error_type removeService(const Service& service);
	 
	 Server* const findServer(const Server::Name& name);
	 const Error::error_type addServer(Server& server);
	 const Error::error_type removeServer(const Server& server);
	 
	 Network* const findNetwork(const Network::Name& name);
	 const Error::error_type addNetwork(Network& network);
	 const Error::error_type removeNetwork(const Network& network);
	 	
	 Channel* const findChannel(const Channel::Name& name);
	 const Error::error_type addChannel(Channel& channel);
	 const Error::error_type removeChannel(const Channel& channel);
      }; // class Registry

      
      // Lazy reference function
      inline static Registry& registry(void)
	{ return Registry::getInstance(); };
   }; // namespace Internal
}; // namespace Kine
   
#endif // _SRC_LIB_REGISTRY_H_
