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

#include "kineircd/registry.h"
#include "kineircd/localuser.h"
#include "lib/debug.h"

using namespace Kine;


// Our instance...
Registry* Registry::instance = 0;


/* Registry - Initialise the registry
 * Original 09/04/2001 simonb
 */
Registry::Registry(void)
 : userCountPeak(0),
   localUserCount(0),
   localUserCountPeak(0),
   serviceCountPeak(0),
   localServiceCount(0),
   localServiceCountPeak(0),
   serverCountPeak(0),
   localServerCount(0),
   localServerCountPeak(0),
   networkCountPeak(0),
   localNetworkCount(0),
   localNetworkCountPeak(0),
   channelCountPeak(0)
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
void Registry::initInstance(void)
{
#ifdef KINE_DEBUG_ASSERT
   // We should only ever be called to init the first instance..
   assert(instance == 0);
#else
   
   // Well, since we are not debugging I suppose we should quietly ignore this
   if (instance != 0) {
# ifdef KINE_DEBUG
      debug("Registry::initInstance() - Called when an instance already "
	    "exists");
# endif
      return;
   }
#endif
   
   // Create the new instance
   instance = new Registry();
   
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Registry::initInstance() - Created new instance @ " << 
     instance;
   debug(debugOut.str());
#endif
}


#warning "This file is crappy"
#ifdef EIAFAIEHFIAEFIAIEHFIAEFIHAEIFIAEFIHAEFIAIEFIAEFIUAEIHIAUEFA
/* changeUserNickname - Change a user's nickname (may involve a key change)
 * Original 16/05/2003
 */
const Error::error_type 
  Registry::changeUserNickname(User& user, const ClientName& newNickname)
{
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Registry::changeUserNickname() - Trying to modify user @ " <<
     &user << " nickname '" << user.getName() << "' -> '" << newNickname <<
     '\'';
   debug(debugOut.str());
#endif
   
   // Are the new nickname and the old nickname are pretty much the same thing?
   if (newNickname.IRCtoLower() == user.getNickname().IRCtoLower()) {
      // Bah, we don't care. Go away.
      return Error::NO_ERROR;
   }
   
   // Find this user
   users_type::iterator it = users.find(user.getNickname().IRCtoLower());
   
   // Did we find the user?
   if (it != users.end()) {
      // Destroy the user! MWA HA HAHA HHAE HAEH HAEhAEHah ehaehaehaeheaHAe!!
      (void)users.erase(it);
      
      // Add the user back with its new key
      (void)users.insert(users_type::value_type(newNickname.IRCtoLower(),
						&user));
      
      // Done
      return Error::NO_ERROR;
   }

   // Complain about the user not existing - silly coders.. :(
   return Error::UNREGISTERED_ENTITY;
}


/* addUser - Add the given user
 * Original 08/04/2003
 */
const Error::error_type Registry::addUser(User& entity)
{
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Registry::addUser() - Trying to add user @ " << &entity <<
     " (" << users.size() << " users known)";
   debug(debugOut.str());
#endif

   // Check to make sure the nickname is valid
   Error::error_type validityError;
   if ((validityError = entity.getNickname().checkValidity()) !=
       Error::NO_ERROR) {
      return validityError;
   }
   
   // Make sure the client doesn't already exist..
   if (findClient(entity.getName()) == 0) {
      // Add the user to the list
      (void)users.
	insert(users_type::value_type(entity.getNickname().IRCtoLower(),
				      &entity));

      // If the user count is higher than the peak, fix the peak
      if (users.size() > userCountPeak) {
	 userCountPeak = users.size();
      }
      
      // If this is a local user, we should increase the local user count too
      if (typeid(entity) == typeid(LocalUser)) {
	 // Increase the local user count
	 ++localUserCount;
	 
	 // If the local user count is higher than the peak, fix this peak too
	 if (localUserCount > localUserCountPeak) {
	    localUserCountPeak = localUserCount;
	 }
      }
      
      // Smile..
      return Error::NO_ERROR;
   }
   
#ifdef KINE_DEBUG
   debug("Registry::addUser() - Client already exists (name conflict)");
#endif

   // Complain
   return Error::CLIENT_EXISTS;
}


/* removeUser - Remove the given user
 * Original 08/04/2003
 */
const Error::error_type Registry::removeUser(const User& entity)
{
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Registry::removeUser() - Trying to remove user @ " <<
     &entity << " (" << entity.getName() << ')';
   debug(debugOut.str());
#endif

   // Find the user
   users_type::iterator it = users.find(entity.getNickname().IRCtoLower());
   
   // Make sure we found something
   if (it != users.end()) {
      // Remove the user from the user list
      (void)users.erase(it);
      
      // If this user was a local user, decrease the local user count
      if (typeid(entity) == typeid(LocalUser)) {
	 --localUserCount;
      }
      
      // Done
      return Error::NO_ERROR;
   }
   
#ifdef KINE_DEBUG
   debug("Registry::removeUser() - Given user was not found");
#endif

   // Complain about the user not existing
   return Error::UNREGISTERED_ENTITY;
}
#endif // AKKAFEKEFAHKAKHHAFEAFHFEAHKFAHAFHKAFKHEAFAKAFEKHAEHKAAEF blah


/* findUser - Find the a user by its name
 * Original 08/04/2003
 */
User* const Registry::findUser(const ClientName& name) const
{
   // Look up the given user from the user list
   users_type::const_iterator it = users.find(name.IRCtoLower());
   
   // Make sure we found something..
   if (it != users.end()) {
      return it->second;
   }
   
   // Not found
   return 0;
}


#ifdef AFKUEAFKUAEFHAEFKAEFHAHEFHAEHFAEFHAEFKHAEHAEF
/* addService - Add the given service
 * Original 08/04/2003
 */
const Error::error_type Registry::addService(Service& entity)
{
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Registry::addService() - Trying to add service @ " <<
     &entity << " (" << users.size() << " services known)";
   debug(debugOut.str());
#endif

   // Check to make sure the nickname is valid
   Error::error_type validityError;
   if ((validityError = entity.getNickname().checkValidity()) !=
       Error::NO_ERROR) {
      return validityError;
   }
   
   // Make sure the client doesn't already exist..
   if (findClient(entity.getName()) == 0) {
      // Add the service to the list
      (void)services.
	insert(services_type::value_type(entity.getNickname().IRCtoLower(),
					 &entity));

      // If the service count is higher than the peak, fix the peak
      if (services.size() > serviceCountPeak) {
	 serviceCountPeak = services.size();
      }
      
      // If this is a local service, we should increase the local counter too
      if (/* */ true) {
	 // Increase the local service count
	 ++localServiceCount;
	 
	 // If the local counter is higher than the peak, fix this peak too
	 if (localServiceCount > localServiceCountPeak) {
	    localServiceCountPeak = localServiceCount;
	 }
      }
      
      // All done!
      return Error::NO_ERROR;
   }
   
#ifdef KINE_DEBUG
   debug("Registry::addService() - Client already exists (name conflict)");
#endif

   // Complain
   return Error::CLIENT_EXISTS;
}


/* removeService - Remove the given user
 * Original 08/04/2003
 */
const Error::error_type Registry::removeService(const Service& entity)
{
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Registry::addService() - Trying to remove service @ " <<
     &entity << " (" << entity.getName() << ')';
   debug(debugOut.str());
#endif

   // Find the local user
   services_type::iterator it = 
     services.find(entity.getNickname().IRCtoLower());
   
   // Make sure we found something
   if (it != services.end()) {
      // .. and also from the users list
      (void)services.erase(it);
      
      // If this is a local service, we should decrease the local counter
      if (/* */ true) {
	 // Decrease the local service count
	 --localServiceCount;
      }
      
      // Done
      return Error::NO_ERROR;
   }
   
#ifdef KINE_DEBUG
   debug("Registry::removeService() - Given service was not found");
#endif

   // Complain about the user not existing
   return Error::UNREGISTERED_ENTITY;
}
#endif // KAHKAFHakefkhAEFHkaefaFKHaefkaefkuaef blah


/* findService - Find the a service by its name
 * Original 08/04/2003
 */
Service* const Registry::findService(const ClientName& name) const
{
   // Look up the given service from the service list
   services_type::const_iterator it = services.find(name.IRCtoLower());
   
   // Make sure we found something..
   if (it != services.end()) {
      return it->second;
   }
   
   // Not found
   return 0;
}
