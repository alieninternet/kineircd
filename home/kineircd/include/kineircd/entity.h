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

#ifndef _INCLUDE_KINEIRCD_ENTITY_H_
# define _INCLUDE_KINEIRCD_ENTITY_H_ 1

# include <string>
# include <aisutil/time.h>
# include <kineircd/name.h>

namespace Kine {
   class User;
   
   /*!
    * \brief A generic \e entity
    * 
    * This is a generic representation of an object which might exist on an
    * IRC network. Everything including users, servers, channels, etc, are
    * all an \e entity of some form.
    */
   class Entity {
    private:
      /*!
       * \brief The time this entity was originally created
       * 
       * This is the time the entity 'appeared' on the network.
       * 
       * \see getSignonTime()
       */
      const AIS::Util::Time signonTime;

      /*!
       * \brief Whether or not the entity is 'registered'
       * 
       * This boolean is used internally to easily determine whether or not
       * the entity is registered on the network or not.
       * 
       * \see isRegistered()
       */
      mutable bool registered;
      
    protected:
      /*!
       * \brief Default Constructor
       * 
       * It's recommended to use Entity(const AIS::Util::Time& _signonTime)
       * for explicit initialisation of \a signonTime.
       */
      explicit Entity(void)
	: signonTime(), // Initialise the signonTime as zeros
          registered(false)
	{};
      
      /*!
       * \brief Constructor
       * 
       * You must provide an initialisation value for the \a signonTime field
       * here, as many network protocols, and indeed statistical modules,
       * require this information.
       * 
       * \param _signonTime The time the entity was created, or when it
       *    connected. This time \b must be relevant to the time on the
       *    system that \e KineIRCd is running on. %Kine requires this
       *    relation for consistancy, and simplicity. If the sign-on time
       *    of the entity is intended to be \e now, assign with the value
       *    from Kine::Daemon::getTime().
       */
      explicit Entity(const AIS::Util::Time& _signonTime)
	: signonTime(_signonTime),
          registered(false)
	{};
      
    public:
      //! Destructor
      virtual ~Entity(void)
	{};


      /*!
       * \brief Return whatever is an acceptable name for this entity
       * 
       * This will return a somewhat \e unique name for this entity. For
       * example, a Server will return its server name, while a Client will
       * return its nickname.
       * 
       * \return The \e unique name for this entity
       */
      virtual const Name& getName(void) const = 0;


      /*!
       * \brief Is this entity hidden from the given user?
       *
       * Check if the entity is hidden from the given user. Note that entities
       * are always available for services and other entities as they're
       * considered to be trusted.
       *
       * \param user Check the given \p user for rights to see \e this entity
       * \return Whether this entity is to be hidden from the given \p user
       * \retval true The user doesn't have the rights to see this entity
       * \retval false This entity is visible to the given \p user
       */
      virtual const bool isHiddenFrom(const User& user) const
	{ return false; };


      /*!
       * \brief Return the time this entity <em>signed on</em>
       * 
       * This returns the \a signonTime of this entity. In most cases, this
       * is the time the entity 'signed on' to the network, in that it is
       * the time it was created and propagated.
       * 
       * Most <em>server to server</em> protocols require this information as
       * an aide for conflict resolution, and statistical modules will want
       * this information too.
       * 
       * Since the likely-hood of two IRC servers maintaining the same time
       * is rare, or indeed the same time-zone for that matter, it's likely
       * that protocol modules will want to offset the time somehow. Since
       * some protocols deal with time in very different manners, time
       * offsets are left up to those individual protocol modules to deal
       * with.
       * 
       * \return The time the entity was created, or connected to the network
       */
      const AIS::Util::Time& getSignonTime(void) const
	{ return signonTime; };


      /*!
       * \brief Is the entity registered?
       * 
       * Determine whether or not the entity is currently registered on the
       * network or not. When registered, the entity is considered to be
       * connected to the network, or \e online, and can be found in the
       * Registry, the internal database of entities.
       * 
       * \return Whether the entity is registered or not
       * \retval true The entity is registered (found within the Registry)
       * \retval false The entity is not currently registered
       */
      const bool isRegistered(void) const
	{ return registered; };
   }; // class Entity
}; // namespace Kine

# include <kineircd/user.h>

#endif // _INCLUDE_KINEIRCD_ENTITY_H_
