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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include <typeinfo>

#include "kineircd/localuser.h"
#include "lib/registry.h"
#include "lib/debug.h"

using namespace Kine;


// Our instance...
Internal::Registry* Internal::Registry::instance = 0;


/* Registry - Initialise the registry
 * Original 09/04/2001 simonb
 */
Internal::Registry::Registry(void)
{
#ifdef KINE_DEBUG_ASSERT
   // Are we sane? Please say yes.. please say yes..
   assert(users.empty());
   assert(services.empty());
   assert(servers.empty());
   assert(networks.empty());
   assert(channels.empty());
#endif
}


/* initInstance - Create the single instance of this class
 * Original 07/04/2003 simonb
 */
void Internal::Registry::initInstance(void)
{
#ifdef KINE_DEBUG_ASSERT
   // We should only ever be called to init the first instance..
   assert(instance == 0);
#else
   
   // Well, since we are not debugging I suppose we should quietly ignore this
   if (instance != 0) {
# ifdef KINE_DEBUG
      debug("Internal::Registry::initInstance() - "
	    "Called when an instance already exists");
# endif
      return;
   }
#endif

   // Create the new instance
   instance = new Registry();

#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut <<
     "Internal::Registry::initInstance() - Created new instance @ " << 
     instance;
   debug(debugOut.str());
#endif
}


/* findEntity - Find the given entity to the given entity list
 * Original 08/04/2003 simonb
 */
template < class Tl >
  inline /* typename Tl::value_type::second */ void* const
    findEntity(Tl& list, const Name& name)
{
   // Look up the given entity from the list
   typename Tl::const_iterator it = list.find(name.IRCtoLower());
   
   // Make sure we found something..
   if (it != list.end()) {
      return it->second;
   }
   
   // Not found - return a null pointer
   return 0;
}


/* addEntity - Add the given entity to the given entity list
 * Original 08/04/2003 simonb
 */
template < class Tl, class Te >
  inline const Error::error_type
    addEntity(Tl& list, Te& entity)
{
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut <<
     "Internal::Registry::addEntity() - Trying to add entity @ " << &entity <<
     " to list " << &list <<
     " which already contains " << list.size() <<
     " entities";
   debug(debugOut.str());
#endif
   
   // Check to make sure the name is valid
   Error::error_type validityError;
   if ((validityError = entity.getName().checkValidity()) !=
       Error::NO_ERROR) {
      return validityError;
   }
   
   // Make sure the entity doesn't already exist..
   if (findEntity(list, entity.getName()) == 0) {
      // Add the entity to the list
      (void)list.insert(typename Tl::value_type(entity.getName().IRCtoLower(),
						&entity));
      
      // Smile..
      return Error::NO_ERROR;
   }

#ifdef KINE_DEBUG
   debug("Internal::Registry::addEntity() - "
	 "Entity already exists in list (name conflict)");
#endif
   
   // Complain
   return Error::ENTITY_EXISTS;
}


/* removeEntity - Add the given entity to the given entity list
 * Original 08/04/2003 simonb
 */
template < class Tl, class Te >
  inline const Error::error_type
    removeEntity(Tl& list, const Te& entity)
{
   // Find the user
   typename Tl::iterator it = list.find(entity.getName().IRCtoLower());
   
   // Make sure we found something
   if (it != list.end()) {
      // Remove the user from the user list
      (void)list.erase(it);
      
      // Done! :)
      return Error::NO_ERROR;
   }
   
#ifdef KINE_DEBUG
   debug("Internal::Registry::removeEntity() - Given entity was not found");
#endif

   // Complain about the user not existing
   return Error::UNREGISTERED_ENTITY;
}


/* changeUserNickname - Change a user's nickname (may involve a key change)
 * Original 16/05/2003
 */
const Error::error_type 
  Internal::Registry::changeUserNickname(User& user,
					 const User::Name& newNickname)
{
   // Are the new nickname and the old nickname are pretty much the same thing?
   if (newNickname.IRCtoLower() == user.getNickname().IRCtoLower()) {
      // Bah, we don't care. Go away.
      return Error::NO_ERROR;
   }
   
   // Find this user
   users_type::iterator it = users.find(user.getNickname().IRCtoLower());
   
   // Did we find the user?
   if (it != users.end()) {
      // Check to make sure the new name is actually valid
      Error::error_type validityError;
      if ((validityError = newNickname.checkValidity()) != Error::NO_ERROR) {
	 return validityError;
      }
   
      // Destroy the user! MWAHA HAHAHE HEHA HAHE HAheaehAhaeHAaheHA!! *twitch*
      (void)users.erase(it);
      
      // *cough* *straighten tie* Add the user back with its new key
      (void)users.insert(users_type::value_type(newNickname.IRCtoLower(),
						&user));
      
      // Done
      return Error::NO_ERROR;
   }

   // Complain about the user not existing - silly coders.. :(
   return Error::UNREGISTERED_ENTITY;
}


// Find entities from various lists
User* const Internal::Registry::findUser(const User::Name& name)
{ return (User* const)findEntity(users, name); }
Service* const Internal::Registry::findService(const Service::Name& name)
{ return (Service* const)findEntity(services, name); }
Server* const Internal::Registry::findServer(const Server::Name& name)
{ return (Server* const)findEntity(servers, name); }
Network* const Internal::Registry::findNetwork(const Network::Name& name)
{ return (Network* const)findEntity(networks, name); }
Channel* const Internal::Registry::findChannel(const Channel::Name& name)
{ return (Channel* const)findEntity(channels, name); }


// Add entities to various lists
const Error::error_type Internal::Registry::addUser(User& entity)
{ return addEntity(users, entity); }
const Error::error_type Internal::Registry::addService(Service& entity)
{ return addEntity(services, entity); }
const Error::error_type Internal::Registry::addServer(Server& entity)
{ return addEntity(servers, entity); }
const Error::error_type Internal::Registry::addNetwork(Network& entity)
{ return addEntity(networks, entity); }
const Error::error_type Internal::Registry::addChannel(Channel& entity)
{ return addEntity(channels, entity); }


// Remove entities from various lists
const Error::error_type
  Internal::Registry::removeUser(const User& entity)
{ return removeEntity(users, entity); }
const Error::error_type
  Internal::Registry::removeService(const Service& entity)
{ return removeEntity(services, entity); }
const Error::error_type
  Internal::Registry::removeServer(const Server& entity)
{ return removeEntity(servers, entity); }
const Error::error_type
  Internal::Registry::removeNetwork(const Network& entity)
{ return removeEntity(networks, entity); }
const Error::error_type
  Internal::Registry::removeChannel(const Channel& entity)
{ return removeEntity(channels, entity); }
