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

# include <kineircd/network.h>
# include <kineircd/server.h>
# include <kineircd/channel.h>
# include <kineircd/service.h>
# include <kineircd/user.h>
# include <kineircd/errors.h>


namespace Kine {
   //! The registry, the place where all entities live
   class Registry {
    public:
      //! Connected users list type
      typedef KINE_HASH_MAP_TYPE < Name, User* const > users_type;
      
      //! Connected services list type
      typedef KINE_HASH_MAP_TYPE < Name, Service* const > services_type;
      
      //! Connected servers list type
      typedef KINE_HASH_MAP_TYPE < std::string, Server* const > servers_type;
      
      //! Connected networks list type
      typedef KINE_HASH_MAP_TYPE < std::string, Network* const > networks_type;
      
      //! Known channel list type
      typedef KINE_HASH_MAP_TYPE < std::string, Channel* const > channels_type;

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
      unsigned int userCountPeak;
      unsigned int localUserCount;
      unsigned int localUserCountPeak;
      unsigned int serviceCountPeak;
      unsigned int localServiceCount;
      unsigned int localServiceCountPeak;
      unsigned int serverCountPeak;
      unsigned int localServerCount;
      unsigned int localServerCountPeak;
      unsigned int networkCountPeak;
      unsigned int localNetworkCount;
      unsigned int localNetworkCountPeak;
      unsigned int channelCountPeak;
      
      
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
      Error::error_type addUser(User& entity);
      
      //! Remove the given user
      Error::error_type removeUser(const User& entity);

      //! Find the given user, by its name
      User* const findUser(const Name& name) const;
      

      //! Add the given service
      Error::error_type addService(Service& entity);
      
      //! Remove the given service
      Error::error_type removeService(const Service& entity);

      //! Find the given service, by its name
      Service* const findService(const Name& name) const;
      
      
      //! Find the given client, by its name
      Client* const findClient(const Name& name) const
	{
	   Client* const client = findUser(name);
	   return ((client != 0) ? client : findService(name));
	};
      
      
      //! Add the given server
      Error::error_type addServer(Server& entity)
	{ return Error::UNKNOWN_ERROR; /* temporary */ };
      
      //! Remove the given server
      Error::error_type removeServer(const Server& entity)
	{ return Error::UNKNOWN_ERROR; /* temporary */ };

      //! Find the given server, by its name
      Server* const findServer(const Name& name) const
	{ return 0; /* temporary */ };
      
      
      //! Add the given network
      Error::error_type addNetwork(Network& entity)
	{ return Error::UNKNOWN_ERROR; /* temporary */ };
      
      //! Remove the given network
      Error::error_type removeNetwork(const Network& entity)
	{ return Error::UNKNOWN_ERROR; /* temporary */ };

      //! Find the given network, by its name
      Network* const findNetwork(const Name& name) const
	{ return 0; /* temporary */ };
      
      
      //! Add the given channel
      Error::error_type addChannel(Channel& entity)
	{ return Error::UNKNOWN_ERROR; /* temporary */ };
      
      //! Remove the given channel
      Error::error_type removeChannel(const Channel& entity)
	{ return Error::UNKNOWN_ERROR; /* temporary */ };

      //! Find the given channel, by its name
      Channel* const findChannel(const Name& name) const
	{ return 0; /* temporary */ };
      
      
      //! Return the user list (read only)
      const users_type& getUsers(void) const
	{ return users; };

      //! Return the service list (read only)
      const services_type& getServices(void) const
	{ return services; };
      
      //! Return the server list (read only)
      const servers_type& getServers(void) const
	{ return servers; };
      
      //! Return the network list (read only)
      const networks_type& getNetworks(void) const
        { return networks; };
      
      //! Return the channel list (read only)
      const channels_type& getChannels(void) const
	{ return channels; };
      
      
      //! Return the number of clients on the network
      const unsigned int getClientCount(void) const
	{ return (getUserCount() + getServiceCount()); };
      
      //! Return the peak number of clients on the network
      const unsigned int getClientCountPeak(void) const
	{ return (getUserCountPeak() + getServiceCountPeak()); };
      
      //! Return the number of clients connected locally
      const unsigned int getLocalClientCount(void) const
	{ return (getLocalUserCount() + getLocalServiceCount()); };
      
      //! Return the peak number of clients connected locally
      const unsigned int getLocalClientCountPeak(void) const
	{ return (getLocalUserCountPeak() + getLocalServiceCountPeak()); };
      
      //! Return the number of users on the network
      const unsigned int getUserCount(void) const
	{ return users.size(); };
      
      //! Return the peak number of users connected to the network
      const unsigned int getUserCountPeak(void) const
	{ return userCountPeak; };
      
      //! Return the number of users on this server locally
      const unsigned int getLocalUserCount(void) const
	{ return localUserCount; };
      
      //! Return the peak number of users connected locally
      const unsigned int getLocalUserCountPeak(void) const
	{ return localUserCountPeak; };
      
      //! Return the number of services on the network
      const unsigned int getServiceCount(void) const
	{ return services.size(); };
      
      //! Return the peak number of services connected to the network
      const unsigned int getServiceCountPeak(void) const
	{ return serviceCountPeak; };
      
      //! Return the number of services connected locally
      const unsigned int getLocalServiceCount(void) const
	{ return localServiceCount; };
      
      //! Return the peak number of services connected locally
      const unsigned int getLocalServiceCountPeak(void) const
	{ return localServiceCountPeak; };
      
      //! Return the number of servers on the network
      const unsigned int getServerCount(void) const
	{ return servers.size(); };
      
      //! Return the peak number of servers
      const unsigned int getServerCountPeak(void) const
	{ return serverCountPeak; };

      //! Return the number of servers connected locally
      const unsigned int getLocalServerCount(void) const
	{ return localServerCount; };
      
      //! Return the peak number of servers connected locally
      const unsigned int getLocalServerCountPeak(void) const
	{ return localServerCountPeak; };
      
      //! Return the number of networks on the network
      const unsigned int getNetworkCount(void) const
	{ return networks.size(); };

      //! Return the peak number of networks
      const unsigned int getNetworkCountPeak(void) const
	{ return networkCountPeak; };
      
      //! Return the number of networks conencted locally
      const unsigned int getLocalNetworkCount(void) const
	{ return localNetworkCount; };
      
      //! Return the peak number of networks connected locally
      const unsigned int getLocalNetworkCountPeak(void) const
	{ return localNetworkCountPeak; };
      
      //! Return the number of channels we know about
      const unsigned int getChannelCount(void) const
	{ return channels.size(); };
      
      //! Return the peak number of channels
      const unsigned int getChannelCountPeak(void) const
	{ return channelCountPeak; };
   }; // class Registry
   

   //! Return the current instance of the Registry:: class
   inline static Registry& registry(void)
     { return Registry::getInstance(); };
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_REGISTRY_H_
