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

#ifndef _INCLUDE_KINEIRCD_MYNETWORK_H_
# define _INCLUDE_KINEIRCD_MYNETWORK_H_ 1

# include <kineircd/user.h>
# include <kineircd/service.h>
# include <kineircd/server.h>
# include <kineircd/network.h>
# include <kineircd/channel.h>

namespace Kine {
   /*!
    * \brief Our network instance
    * 
    * This is a special instance of a Network, which corresponds to the network
    * this server is connected to (and therefore represents). This Network
    * instance presents a snapshot of the current network.
    * 
    * You should reference it through Kine::myNetwork().
    */
   class MyNetwork : public Network {
    private:
      counter_type userCount;			//! Total users online
      counter_type serviceCount;		//! Total services online
      counter_type serverCount;			//! Total servers linked
      counter_type networkCount;		//! Total networks linked
      
      //! Our instance
      static MyNetwork* instance;
      
      //! Constructor
      MyNetwork(void);
      
    public:
      //! Destructor
      ~MyNetwork(void)
	{};
      
      // Create the single instance of this class
      static void initInstance(void);
      
      // Return out instance
      static MyNetwork& getInstance(void)
	{ return *instance; };
      
      /*!
       * \brief Determine whether our network name is available
       * 
       * The network name is not a requirement for stand-alone servers, or
       * even stand-alone networks. As such, network names may not be
       * configured.
       * 
       * To determine reliably whether the network name is configured or not,
       * you should call this function rather than relying on the network name
       * being empty.
       * 
       * \return Whether the network name is set or not
       */
      const bool isNamed(void) const
	{ return !getName().empty(); };


      /*!
       * \brief Find the given user, using its name
       * 
       * \param name The name of the user to find
       * \return A pointer to the User requested
       * \retval 0
       *    The user could not be found
       */
      User* const findUser(const Client::Name& name) const;

      /*!
       * \brief Find the given service, using its name
       * 
       * \param name The name of the service to find
       * \return A pointer to the Service requested
       * \retval 0
       *    The service could not be found
       */
      Service* const findService(const Client::Name& name) const;

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
      Client* const findClient(const Client::Name& name) const
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
      Server* const findServer(const Server::Name& name) const;
      
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
      Network* const findNetwork(const Network::Name& name) const;
      
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
      Channel* const findChannel(const Channel::Name& name) const;


      //@{
      /*!
       * \brief Return the number of clients on this network
       * 
       * Note that this is simply a combination of the number (and peak
       * number) of users and services on this network.
       * 
       * \return The number of clients on this network
       */
      const counter_type getClientCount(void) const
	{ return (getUserCount() + getServiceCount()); };
      
      /*!
       * \brief Return the number of users on this network
       * 
       * \return The number of users on this network
       */
      const counter_type& getUserCount(void) const
	{ return userCount; };
      
      /*!
       * \brief Return the number of services on this network
       *
       * \return The number of services on this network
       */
      const counter_type& getServiceCount(void) const
	{ return serviceCount; };
      
      /*!
       * \brief Return the number of servers on this network
       * 
       * \return The number of servers on this network
       */
      const counter_type& getServerCount(void) const
	{ return serverCount; };

      /*!
       * \brief Return the number of networks on this network
       * 
       * \return The number of networks on this network
       */
      const counter_type& getNetworkCount(void) const
	{ return networkCount; };
      
      /*! 
       * \brief Return the known number of channels available on this network
       * 
       * This returns the total number of channels visible to us on this
       * network. This will include all the channels of all distribution
       * scopes to the best we can, but obviously cannot include all channels
       * available on the network because some of them are very tightly
       * scoped (i.e. locally scoped channels which only exist on one server,
       * and are not on MyServer()).
       *
       * Likewise, the number of channels may also not reflect what another
       * server will believe, considering there may be channels scoped
       * locally on this server which are not distributed to other servers.
       * 
       * It may seem odd, but internetwork-scoped channels (which span
       * multiple networks) are also included in this count. Ultimately, this
       * count contains the number of channels \e on this network, and not the
       * number of channels \e within this network. Therefore, this count
       * represents the number of channels \e available to the network as
       * seen by this part of the network.
       * 
       * \return The number of channels on this network (as visible)
       */
      const counter_type getChannelCount(void) const
	{ return counter_type(); /* fix me - zeroed for now */ };
      //@}
   }; // class MyNetwork


   /*!
    * \brief Return the MyNetwork instance
    *
    * Since MyNetwork is a singleton, this is how you should reference it in
    * your code.
    *
    * \return The current instance of MyNetwork
    * \relates Kine::MyNetwork
    */
   inline static MyNetwork& myNetwork(void)
     { return MyNetwork::getInstance(); };
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_MYNETWORK_H_
