/* $Id$
 * The registry, the place where all entities live
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

#include "kineircd/registry.h"
#include "libkineircd/debug.h"

using namespace Kine;


// Our instance...
Registry* Registry::instance = 0;


/* Registry - Initialise the registry
 * Original 09/04/2001 simonb
 */
Registry::Registry(void)
{
#ifdef KINE_DEBUG_ASSERT
   // Are we sane? Please say yes.. please say yes..
   assert(users.empty());
   assert(localUsers.empty());
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


/* add - Add the given user
 * Original 08/04/2003
 */
Error::error_type Registry::add(User& entity)
{
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Registry::add() - Trying to add user @ " << &entity <<
     " (" << users.size() << " users known)";
   debug(debugOut.str());
#endif

   // Make sure the client doesn't already exist..
   if (findClient(entity.getNickname()) == 0) {
      // Add the user to the list
      (void)users.
	insert(users_type::value_type(entity.getNickname().IRCtoLower(),
				      &entity));
      
      // Smile..
      return Error::NO_ERROR;
   }
   
#ifdef KINE_DEBUG
   debug("Registry::add() - Client already exists (name conflict)");
#endif

   // Complain
   return Error::CLIENT_EXISTS;
}


/* add - Add the given local user
 * Original 08/04/2003
 */
Error::error_type Registry::add(LocalUser& entity)
{
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Registry::add() - Trying to add local user @ " << &entity <<
     " (" << localUsers.size() << " local users, " << users.size() << 
     " users known)";
   debug(debugOut.str());
#endif

   // Make sure the client doesn't already exist..
   if (findClient(entity.getNickname()) == 0) {
      // Add the user to the local user list..
      (void)localUsers.
	insert(localUsers_type::value_type(entity.getNickname().IRCtoLower(),
					   &entity));
      
      // .. And to the user list..
      (void)users.
	insert(users_type::value_type(entity.getNickname().IRCtoLower(),
				      &entity));
      
      // All is well :)
      return Error::NO_ERROR;
   }
   
#ifdef KINE_DEBUG
   debug("Registry::add() - Client already exists (name conflict)");
#endif

   // Complain
   return Error::CLIENT_EXISTS;
}


/* remove - Remove the given user
 * Original 08/04/2003
 */
Error::error_type Registry::remove(const User& entity)
{
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Registry::add() - Trying to remove user @ " << &entity;
   debug(debugOut.str());
#endif

   // Find the local user
   users_type::iterator it = users.find(entity.getNickname().IRCtoLower());
   
   // Make sure we found something
   if (it != users.end()) {
      // .. and also from the users list
      (void)users.erase(it);
      
      // Done
      return Error::NO_ERROR;
   }
   
#ifdef KINE_DEBUG
   debug("Registry::remove() - Given user was not found");
#endif

   // Complain about the user not existing
   return Error::UNREGISTERED_ENTITY;
}


/* remove - Remove the given local user
 * Original 08/04/2003
 */
Error::error_type Registry::remove(const LocalUser& entity)
{
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Registry::add() - Trying to remove local user @ " << &entity;
   debug(debugOut.str());
#endif

   // Find the local user
   localUsers_type::iterator it =
     localUsers.find(entity.getNickname().IRCtoLower());
   
   // Make sure we found something
   if (it != localUsers.end()) {
      // Remove the user from the local user list
      (void)localUsers.erase(it);

      // Remove it from the global user list too..
      return remove((const User&)entity);
   }
   
#ifdef KINE_DEBUG
   debug("Registry::remove() - Given local user was not found");
#endif

   // Complain about the user not existing
   return Error::UNREGISTERED_ENTITY;
}


/* findUser - Find the a user by its name
 * Original 08/04/2003
 */
User* const Registry::findUser(const Name& name) const
{
   // Look up the given use from the local user list
   users_type::const_iterator it = users.find(name.IRCtoLower());
   
   // Make sure we found something..
   if (it != users.end()) {
      return it->second;
   }
   
   // Not found
   return 0;
}


/* findLocalUser - Find the a local user by its name
 * Original 08/04/2003
 */
LocalUser* const Registry::findLocalUser(const Name& name) const
{
   // Look up the given use from the local user list
   localUsers_type::const_iterator it = localUsers.find(name.IRCtoLower());
   
   // Make sure we found something..
   if (it != localUsers.end()) {
      return it->second;
   }
   
   // Not found
   return 0;
}
