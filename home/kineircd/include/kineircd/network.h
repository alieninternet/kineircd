/* $Id$
 * 
 * Copyright (c) 2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002,2003 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_NETWORK_H_
# define _INCLUDE_KINEIRCD_NETWORK_H_ 1

# include <kineircd/entity.h>
# include <kineircd/sender.h>

namespace Kine {
   /*!
    * \brief A network
    * 
    * This is literally an IRC network.
    */
   class Network : public Entity {
    protected:
      /*!
       * \brief Constructor
       * 
       * \param _signonTime The time the Network connected. For more
       *    information, see Kine::Entity::signonTime.
       */
      explicit Network(const AIS::Util::Time& _signonTime)
	: Entity(_signonTime)
	{};
      
    public:
      //! Destructor
      virtual ~Network(void)
	{};

      /*!
       * \brief Get the network's short name
       * 
       * Return the network name, in <em>short form</em>. For example,
       * <em>"The Foo Network"</em> would probably use something along the
       * lines of \e "FooNET" as their short name.
       * 
       * \return The short network name
       */
      virtual const std::string& getNetworkName(void) const = 0;
      
      // Return the 'name'
      const std::string& getName(void) const
	{ return getNetworkName(); };
   }; // class Network
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_NETWORK_H_
