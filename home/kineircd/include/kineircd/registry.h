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

#ifndef _INCLUDE_KINEIRCD_REGISTRY_H_
# define _INCLUDE_KINEIRCD_REGISTRY_H_ 1

# ifdef KINE_STL_HAS_HASH
#  include <hash_map>
# endif
# include <map>

# include <kineircd/user.h>
# include <kineircd/service.h>
# include <kineircd/server.h>
# include <kineircd/network.h>
# include <kineircd/channel.h>
# include <kineircd/errors.h>


namespace Kine {
   //! The registry, the place where all entities live
   class Registry {
    public:
      //! The 'counter' type, used for various statistical replies
      typedef unsigned int counter_type;

    private:
      //! Connected users list type
      typedef KINE_HASH_MAP_TYPE < ClientName, User* const > users_type;
      
      //! Connected services list type
      typedef KINE_HASH_MAP_TYPE < ClientName, Service* const > services_type;
      
      //! Connected servers list type
      typedef KINE_HASH_MAP_TYPE < std::string, Server* const > servers_type;
      
      //! Connected networks list type
      typedef KINE_HASH_MAP_TYPE < std::string, Network* const > networks_type;
      
      //! Known channel list type
      typedef KINE_HASH_MAP_TYPE < ChannelName, Channel* const > channels_type;

    private:
      //! Connected users list
      users_type users;

      //! Connected services list
      services_type services;
      
      //! Connected servers list
      servers_type servers;
      
      //! Connected networks list
      networks_type networks;
      
      //! Known channel list
      channels_type channels;
      

      //! Various statistics counters/peak levels
      counter_type userCountPeak;
      counter_type localUserCount;
      counter_type localUserCountPeak;
      counter_type serviceCountPeak;
      counter_type localServiceCount;
      counter_type localServiceCountPeak;
      counter_type serverCountPeak;
      counter_type localServerCount;
      counter_type localServerCountPeak;
      counter_type networkCountPeak;
      counter_type localNetworkCount;
      counter_type localNetworkCountPeak;
      counter_type channelCountPeak;
      
      
      //! Our single instance
      static Registry* instance;
      
      //! Constructor
      Registry(void);

      
      //! Change a user's nickname
      const Error::error_type 
	changeUserNickname(User& user, const ClientName& newNickname);

      // Our friends
      friend class User;
      
    public:
      // Destructor
      ~Registry(void)
	{};
      
      // Create the single instance of this class
      static void initInstance(void);
      
      // Get the single instance of this class
      static Registry& getInstance(void)
	{ return *instance; };
      
      
      // Add the given user
      const Error::error_type addUser(User& entity);
      
      // Remove the given user
      const Error::error_type removeUser(const User& entity);
      
      // Add the given service
      const Error::error_type addService(Service& entity);
      
      // Remove the given service
      const Error::error_type removeService(const Service& entity);


      //! Find the given user, by its name
      User* const findUser(const ClientName& name) const;

      //! Find the given service, by its name
      Service* const findService(const ClientName& name) const;

      //! Find the given client, by its name (check both user and service lists)
      Client* const findClient(const ClientName& name) const
	{
	   Client* const client = findUser(name);
	   return ((client != 0) ? client : findService(name));
	};
      
      //! Find the given server, by its name
      Server* const findServer(const std::string& name) const
	{ return 0; /* temporary */ };
      
      //! Find the given network, by its name
      Network* const findNetwork(const std::string& name) const
	{ return 0; /* temporary */ };
      
      //! Find the given channel, by its name
      Channel* const findChannel(const ChannelName& name) const
	{ return 0; /* temporary */ };
      
      
      //! Return the number of clients on the network
      const counter_type getClientCount(void) const
	{ return (getUserCount() + getServiceCount()); };
      
      //! Return the peak number of clients on the network
      const counter_type getClientCountPeak(void) const
	{ return (getUserCountPeak() + getServiceCountPeak()); };
      
      //! Return the number of clients connected locally
      const counter_type getLocalClientCount(void) const
	{ return (getLocalUserCount() + getLocalServiceCount()); };
      
      //! Return the peak number of clients connected locally
      const counter_type getLocalClientCountPeak(void) const
	{ return (getLocalUserCountPeak() + getLocalServiceCountPeak()); };
      
      //! Return the number of users on the network
      const counter_type getUserCount(void) const
	{ return users.size(); };
      
      //! Return the peak number of users connected to the network
      const counter_type getUserCountPeak(void) const
	{ return userCountPeak; };
      
      //! Return the number of users on this server locally
      const counter_type getLocalUserCount(void) const
	{ return localUserCount; };
      
      //! Return the peak number of users connected locally
      const counter_type getLocalUserCountPeak(void) const
	{ return localUserCountPeak; };
      
      //! Return the number of services on the network
      const counter_type getServiceCount(void) const
	{ return services.size(); };
      
      //! Return the peak number of services connected to the network
      const counter_type getServiceCountPeak(void) const
	{ return serviceCountPeak; };
      
      //! Return the number of services connected locally
      const counter_type getLocalServiceCount(void) const
	{ return localServiceCount; };
      
      //! Return the peak number of services connected locally
      const counter_type getLocalServiceCountPeak(void) const
	{ return localServiceCountPeak; };
      
      //! Return the number of servers on the network
      const counter_type getServerCount(void) const
	{ return servers.size(); };
      
      //! Return the peak number of servers
      const counter_type getServerCountPeak(void) const
	{ return serverCountPeak; };

      //! Return the number of servers connected locally
      const counter_type getLocalServerCount(void) const
	{ return localServerCount; };
      
      //! Return the peak number of servers connected locally
      const counter_type getLocalServerCountPeak(void) const
	{ return localServerCountPeak; };
      
      //! Return the number of networks on the network
      const counter_type getNetworkCount(void) const
	{ return networks.size(); };

      //! Return the peak number of networks
      const counter_type getNetworkCountPeak(void) const
	{ return networkCountPeak; };
      
      //! Return the number of networks conencted locally
      const counter_type getLocalNetworkCount(void) const
	{ return localNetworkCount; };
      
      //! Return the peak number of networks connected locally
      const counter_type getLocalNetworkCountPeak(void) const
	{ return localNetworkCountPeak; };
      
      //! Return the number of channels we know about
      const counter_type getChannelCount(void) const
	{ return channels.size(); };
      
      //! Return the peak number of channels
      const counter_type getChannelCountPeak(void) const
	{ return channelCountPeak; };
   }; // class Registry
   

   /*!
    * \brief Return the current instance of the Registry:: class
    * 
    * \return The current instance of the Registry class
    * \relates Kine::Registry
    */
   inline static Registry& registry(void)
     { return Registry::getInstance(); };
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_REGISTRY_H_
