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
      typedef KINE_HASH_MAP_TYPE < std::wstring, Server* const > servers_type;
      
      //! Connected networks list type
      typedef KINE_HASH_MAP_TYPE < std::wstring, Network* const > networks_type;
      
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

      
      /*-------------------------------------------------------*/
      // Remove these soon - do not use these in your own code
      const Error::error_type removeUser(const User& entity);
      const Error::error_type addUser(User& entity);
      const Error::error_type changeUserNickname(User& user, const ClientName& newNickname);
      /*-------------------------------------------------------*/

      
      /*!
       * \brief Find the given user, using its name
       * 
       * \param name The name of the user to find
       * \return A pointer to the User requested
       * \retval 0
       *    The user could not be found
       */
      User* const findUser(const ClientName& name) const;

      /*!
       * \brief Find the given service, using its name
       * 
       * \param name The name of the service to find
       * \return A pointer to the Service requested
       * \retval 0
       *    The service could not be found
       */
      Service* const findService(const ClientName& name) const;

      /*!
       * \brief Find the given client, using its name
       *
       * This works in an identical fashion to findUser() and findService(),
       * except that it searches both lists. Since users are more common than
       * services (obviously), the user list is checked first.
       * 
       * \param name The name of the client to find
       * \return A pointer to the Client requested
       * \retval 0
       *    The client could not be found
       */
      Client* const findClient(const ClientName& name) const
	{
	   Client* const client = findUser(name);
	   return ((client != 0) ? client : findService(name));
	};
      
      /*!
       * \brief Find the given server, using its name
       * 
       * \param name The name of the server to find 
       * \return The instance of the Server requested
       * \retval 0
       *    The server could not be found
       */
      Server* const findServer(const std::wstring& name) const;
      
      /*!
       * \brief Find the given network, using its name
       * 
       * Note that the network list is most likely to be empty is there is
       * no network name connected. Networks cannot be interlinked unless this
       * network has a name.
       *
       * \see MyNetwork::isNamed()
       * \param name The name of the network to find
       * \return The instance of the Network requested
       * \retval 0
       *    The network could not be found
       */
      Network* const findNetwork(const std::wstring& name) const;
      
      /*!
       * \brief Find the given channel, using its name
       * 
       * The channel name needs to include its type-prefix character, as it
       * is an integral part of the channel's name.
       * 
       * \param name The name of the channel to find
       * \return The instance of the Channel requested
       * \retval 0
       *    The channel could not be found
       */
      Channel* const findChannel(const ChannelName& name) const;
      
      
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
