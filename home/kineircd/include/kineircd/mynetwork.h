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

# include <kineircd/network.h>

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
