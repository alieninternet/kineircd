/* $Id$
 * A locally connected user, using the IRC-2 server <-> user protocol
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

#ifndef _SRC_MOD_IRC2USER_USER_H_
# define _SRC_MOD_IRC2USER_USER_H_ 1

# include <string>
# include <kineircd/localuser.h>
# include <kineircd/registrant.h>

namespace Kine {
   namespace mod_irc2user {
      class User : public Kine::LocalUser {
       private:
	 std::string description;		// Description/real name field
	 timeval lastAwake;			// Time user was last awake
	 
       public:
	 // Constructor
	 User(const Kine::Registrant& _registrant)
	   : LocalUser(_registrant.name,
		       _registrant.username,
		       _registrant.hostname),
	     description(_registrant.description),
	     lastAwake(Kine::daemon().getTime())
	   {};
	 
	 // Destructor
	 ~User(void)
	   {};
	 
	 // Return the description string
	 const std::string& getDescription(void) const
	   { return description; };
	 
	 // Return the time the user was last 'awake'
	 const timeval& getLastAwake(void) const
	   { return lastAwake; };
	 
	 // Prod the last awake field with the current time
	 void updateLastAwake(void)
	   { lastAwake = Kine::daemon().getTime(); };
      }; // class User
   }; // namespace mod_irc2user
}; // namespace Kine
   
#endif // _SRC_MOD_IRC2USER_USER_H_
   
