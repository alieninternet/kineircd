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

#ifndef _INCLUDE_KINEIRCD_DENIZEN_H_
# define _INCLUDE_KINEIRCD_DENIZEN_H_ 1

# include <kineircd/entity.h>

namespace Kine {
   class Server;
   
   /*!
    * \brief Base class for \e denizens
    * 
    * A denizen is a special type of Entity which can connect to a server,
    * such as a Client or a Server itself. Denizens are named such because
    * they literally do \e inhabit servers, rather than merely \e existing
    * on a server.
    * 
    * Since denizens connect to servers, they also have additional information
    * about themselves such as where they're connected from, and so forth.
    */
   class Denizen : virtual public Entity {
    public:
      /*!
       * \brief Hop count type
       *
       * This is a type definition for hop counts.
       */
      typedef unsigned int hopCount_type;
      
    protected:
      //! Constructor
      Denizen(void)
	{};
      
    public:
      //! Destructor
      virtual ~Denizen(void)
	{};

      /*!
       * \brief Return the hostname
       * 
       * This returns the hostname that the denizen is connected from, which
       * may be resolved by \e DNS or some other means, but could also be a
       * raw network address. If the hostname is a network address,
       * represented as a human-readable string, then the address is dependant
       * on the network protocol being used.
       * 
       * \e IPv4 addresses will look like <em>123.4.567.89</em>, or
       * <em>::ffff:123.4.567.89</em>, depending on the socket type used. The
       * latter form is an \e IPv4 connection to an \e IPv6 socket.
       * 
       * \e IPv6 addresses will look something like
       * <em>3ffe:124::4c01:53a1</em>, although it's more probable for
       * something longer, such as
       * <em>2001:470:1f00:193:1091:20:f3d9:ba8f</em>.
       * 
       * \e IPX addresses will look like <em>00ABC123:004F4C91BA6C</em>.
       * 
       * Addresses resolved from \e DNS will look something like
       * <em>some.hostname.foo.com</em>.
       * 
       * Other network address types may be available, depending on the
       * protocol used and the type of entity.
       * 
       * \return The hostname representing the host of the denizen
       */
      virtual const std::string& getHostname(void) const = 0;
      
      /*!
       * \brief Return the denizen's \e description field
       *
       * Every denizen has a \e description field, used for such things as
       * the description of a Service, the real name of a User, a \e URL or
       * name of a Server sponsor/donor, or the full name of a Network.
       * 
       * Depending on the terminology and denizen type you're used to, this
       * is sometimes also referred to as a <em>real name</em> or \e GECOS
       * field.
       * 
       * \return The \e description of the denizen
       */
      virtual const std::string& getDescription(void) const = 0;

      /*!
       * \brief Return the number of hops from \e us to this denizen
       * 
       * This is the number of hops from this server (or technically, 
       * MyServer) to this denizen, though the <em>IRC network</em>
       * internally.
       * 
       * For example, if the denizen is connected directly to \e this server,
       * the number of hops would be \e 0. If a user was connected to a server
       * which was in turn connected to this server, the hop count would be
       * \e 1, and so on and so forth.
       * 
       * \return The number of hops from here to the denizen though the
       *    network
       * \retval 0 The denizen is connected to this server
       */
      virtual const hopCount_type getHopCount(void) const
	{ return 0; };
      
      /*!
       * \brief Return a the server this denizen is connected to
       * 
       * This returns the server the denizen is currently \e most connected
       * to. This definition is such because some connections, such as
       * <em>server to server</em> links, may have redundant connections to
       * the network.
       * 
       * Denizens locally connected will simply return a reference to the
       * current instance of MyServer (and that includes MyServer itself).
       * 
       * \return The server the denizen is connected to
       */
      virtual Server& getServer(void) const = 0;
   }; // class Denizen
}; // namespace Kine

# include <kineircd/server.h>

#endif // _INCLUDE_KINEIRCD_DENIZEN_H_
